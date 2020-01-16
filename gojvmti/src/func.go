package main

import "C"
import (
	"fmt"
	"strconv"
)

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
