/**
 * main.c
 *
 * Detecting memory leaks only for windows .  
 * Place the following snippet where leak to be tested: 
 * #if defined(_CRTDBG_MAP_ALLOC) 
 *    _CrtDumpMemoryLeaks(); 
 * #endif 
 */
#if defined(WIN32) && defined(_DEBUG)
  #ifndef   _CRTDBG_MAP_ALLOC  
    #pragma message( __FILE__": _CRTDBG_MAP_ALLOC defined only for DEBUG on Win32." )   
    #define _CRTDBG_MAP_ALLOC  
    #include<stdlib.h>   
    #include<crtdbg.h>  
  #endif  
#endif  

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "map.h"

typedef struct counter_t {
  char  name[20];
  int   number;
} counter;

static void print_counters(int c) {
  printf("value=%d\n", c);
}


int main(int argc, char* argv[])
{
  char  *name;
  int ret, i, j;

  /* create hashmap */
  hmap_t map = hashmap_create();
  printf("hashmap_created: %d\n", hashmap_size(map));

  /* add hashmap node */
  for (i=0; i<20; i++) {
    sprintf(name, "%d", i);
    ret = hashmap_put(map, name, i);
    assert(ret==HMAP_S_OK);
  }
  printf("hashmap_loaded: %d\n", hashmap_size(map));

  /* rm key="10" */
  ret = hashmap_remove(map, "10", j);
  assert(ret==HMAP_S_OK);
  printf("hashmap_remove: name=%s. size=%d\n", name, hashmap_size(map));
  hashmap_iterate(map, print_counters, 0);

  /* get key="9" */
  ret = hashmap_get(map, "9", j);
  assert(ret==HMAP_S_OK);
  printf("hashmap_get: name=%s. size=%d\n", name, hashmap_size(map));

  /* rm all map */
  //hashmap_destroy(map, free_map, 0);

  //_CrtDumpMemoryLeaks();
  return 0;
}

