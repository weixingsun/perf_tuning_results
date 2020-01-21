package main

import "C"
import (
	"strconv"
	"path/filepath"
)
var JVMTYPES = make(map[int8]string)
//export gJVMTypeInit
func gJVMTypeInit() {
	JVMTYPES['B'] = "[byte]";
	JVMTYPES['C'] = "[char]";
	JVMTYPES['S'] = "[short]";
	JVMTYPES['I'] = "[int]";
	JVMTYPES['J'] = "[long]";
	JVMTYPES['F'] = "[float]";
	JVMTYPES['D'] = "[double]";
	JVMTYPES['Z'] = "[boolean]";
}

//export goAtoi
func goAtoi(c *C.char) int {
	i,_:=strconv.ParseInt(C.GoString(c), 0, 32)
	return int(i)
}

//export gStringAdd
//func gStringAdd(c1 *C.char,c2 *C.char,s *C.char) *C.char {
	//c:=C.CString(C.GoString(c1)+C.GoString(s)+C.GoString(c2))
	//defer C.free(unsafe.Pointer(c))
	//return C.CString(C.GoString(c1)+C.GoString(s)+C.GoString(c2))
//}

//export gLog
//func gLog(c *C.char) {
	//fmt.Printf("gLog: [%v] \n", C.GoString(c))
//}

//export gTranslateJVMType
//func gTranslateJVMType(c C.int) *C.char {
	//return C.CString( JVMTYPES[int8(c)] )
//}

//export gLastName
func gLastName(c *C.char) *C.char {
	s:=filepath.Base(C.GoString(c))
	return C.CString( s[:len(s)-1] )
}

