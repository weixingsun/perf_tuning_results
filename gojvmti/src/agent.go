package main

//#include <stdio.h>
//#include <stdlib.h>
//#include <jvmti.h>
import "C"
import (
	"strconv"
	//"unsafe"
)

var counters = NewCounter()

//export gCacheMapCount
func gCacheMapCount() {
	counters.GetAllCount()
}

//export gCacheMapClean
func gCacheMapClean() {
	counters.Clean()
}
//export gCacheObject
func gCacheObject(method *C.char,object *C.char, size C.int) {
	m := C.GoString(method)
	o := C.GoString(object)
	s := strconv.Itoa(int(size))
	k := m+"()"+o+"["+s+"]"
	counters.Inc(k)
}

func main() {}