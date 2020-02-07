package main

//#cgo CFLAGS: -I. -O3 -Ofast -march=native
//#cgo LDFLAGS:-L. -ldl -Wl,--no-as-needed
//#include <stdio.h>
//#include <stdlib.h>
//#include <jvmti.h>
import "C"
import (
	"strconv"
	//"unsafe"
)

var counters = NewCounter()


//export gCacheObject
func gCacheObject(method *C.char,object *C.char, size C.int) {
	m := C.GoString(method)
	o := C.GoString(object)
	s := strconv.Itoa(int(size))
	k := m+"()"+o+"["+s+"]"
	counters.Inc(k)
}

func main() {}

//export gCacheMapCount
//func gCacheMapCount() {
//	counters.GetAllCount()
//}

//export gCacheMapClean
//func gCacheMapClean() {
//	counters.Clean()
//}