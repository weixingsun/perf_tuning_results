package main

//#include <stdio.h>
//#include <stdlib.h>
//#include <jvmti.h>
//#include "api.h"
import "C"
import (
	"fmt"
	"strconv"
	"strings"
	"errors"
	//"unsafe"
)

var counters = NewCounter()

func gConfig(s string) (map[string]int, error) {
	ss := strings.Split(s, ",")
	m := make(map[string]int)
	for _, pair := range ss {
		z := strings.Split(pair, "=")
		if len(z)<2 {
			return nil,errors.New("Runtime Error")
		}
		i,e:=strconv.ParseInt(z[1], 0, 32)
		if e != nil {
			return nil,e
		}
		m[z[0]] = int(i)
	}
	return m,nil
}

//export gOptions
func gOptions(jvmti *C.jvmtiEnv, co *C.char) {
	opt := C.GoString(co)
	m,err := gConfig(opt)
	if err != nil {
	    fmt.Printf("Invalid Agent Options: %v\n", opt)
	}else{
	    fmt.Printf("Agent options: [%v] \n", m)
	}
	C.cSetHeapSamplingInterval( jvmti, C.int(m["interval"]) )
}

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