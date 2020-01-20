#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "hashmap.h"

void set(map_t mymap, char* key, int v){
	data_struct_t* value = malloc(sizeof(data_struct_t));
	value->key_string = key;
    value->number = v;
	int error = hashmap_put(mymap, value->key_string, value);
    assert(error==MAP_OK);
	//printf("set key: %s  value: %d\n",key,v);
}
int get(map_t mymap, char* key){
	data_struct_t* value = malloc(sizeof(data_struct_t));
	int error = hashmap_get(mymap, key, (void**)(&value));
	//printf("get key: %s  value: %d\n",key,value->number);
	if (error==MAP_MISSING){
		return -1;
	}else{
		return value->number;
	}
}
void inc(map_t mymap, char* key){
    //printf("---------------------------------------------\n");
	//get(mymap, key);
    //printf("before_inc: map_size=%d\n", hashmap_length(mymap));
	int error = hashmap_inc(mymap, key);
    //printf("inc key:   %s  \n",key);
	assert(error==MAP_OK);
    //printf("after_inc:  map_size=%d\n", hashmap_length(mymap));
	//get(mymap, key);
    //printf("---------------------------------------------\n");
}
void rm(map_t mymap, char* key){
	int error = hashmap_remove(mymap, key);
	assert(error==MAP_OK);
	//free(value);
}
int main(int argc, char** argv){

    map_t mymap = hashmap_new();
    printf("hashmap_created: size=%d\n", hashmap_length(mymap));
	
	//snprintf(key_string, KEY_MAX_LENGTH, "%s%d", KEY_PREFIX, index);
	set(mymap, "node0", 0);
	set(mymap, "node1", 1);
	set(mymap, "node2", 2);
	set(mymap, "node3", 3);
    printf("hashmap_put: size=%d\n", hashmap_length(mymap));
	
	char* k = "nodeX";
    int i = get(mymap, k);
    printf("hashmap_get not found: key=%s  value:%d\n", k,i);
    printf("hashmap size=%d\n", hashmap_length(mymap));
	
	k = "node2";
	i = get(mymap, k);
	assert(i==2);
	///////////////////////////////////////////////////////////////////////
	printf("before inc : key=%s  value:%d\n", k,i);
	inc(mymap, k);
    i = get(mymap, k);
	printf("after inc : key=%s  value:%d\n", k,i);
	///////////////////////////////////////////////////////////////////////
	k = "nodeX";
	printf("before inc : key=%s size=%d\n", k, hashmap_length(mymap));
	inc(mymap, k);
    i = get(mymap, k);
	printf("after inc : key=%s  value:%d\n", k,i);
	///////////////////////////////////////////////////////////////////////
    printf("before rm: size=%d\n", hashmap_length(mymap));
	hashmap_empty(mymap);
    printf("---------------------------------------------\n");
	hashmap_print(mymap);
	printf("after rm: size=%d\n", hashmap_length(mymap));
	
	set(mymap, "node0", 0);
	set(mymap, "node1", 1);
	set(mymap, "node2", 2);
	set(mymap, "node3", 3);
    printf("hashmap_put: size=%d\n", hashmap_length(mymap));
	
    printf("---------------------------------------------\n");
	hashmap_print(mymap);
    hashmap_free(mymap);

    return 1;
}
