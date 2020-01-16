package main

//#include <stdio.h>
//#include <stdlib.h>
//#include <jvmti.h>
import "C"
import (
	"fmt"
	"strconv"
	//"unsafe"
)

var counters = NewCounter()

//export goAtoi
func goAtoi(c *C.char) int {
	i,_:=strconv.ParseInt(C.GoString(c), 0, 32)
	return int(i)
}

//export gStringAdd
func gStringAdd(c1 *C.char,c2 *C.char,s *C.char) *C.char {
	//c:=C.CString(C.GoString(c1)+C.GoString(s)+C.GoString(c2))
	//defer C.free(unsafe.Pointer(c))
	return C.CString(C.GoString(c1)+C.GoString(s)+C.GoString(c2))
}

//export gLog
func gLog(c *C.char) {
	fmt.Printf("gLog: [%v] \n", C.GoString(c))
}

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