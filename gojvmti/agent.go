package main

//#include <jvmti.h>
//#include "api.h"
import "C"
import (
	"fmt"
	"strconv"
	"strings"
)

//interval=1024,stacktrace=0
func gMap(s string) map[string]int {
	var m map[string]int
	var ss []string

	//fmt.Printf("gMap: [%v] \n", s)
	ss = strings.Split(s, ",")
	m = make(map[string]int)
	for _, pair := range ss {
		z := strings.Split(pair, "=")
		i,_:=strconv.ParseInt(z[1], 0, 32)
		m[z[0]] = int(i)
	}
	return m
}
//////////////////////////////////////////////////

//export gOptions
func gOptions(jvmti *C.jvmtiEnv, co *C.char) {
	opt := C.GoString(co)
	//interval=N,stacktrace=F
	m := gMap(opt)
	fmt.Printf("Agent options: [%v] \n", m)
	C.cSetHeapSamplingInterval( jvmti, C.int(m["interval"]) )
}
//export goAtoi
func goAtoi(c *C.char) int {
	i,_:=strconv.ParseInt(C.GoString(c), 0, 32)
	//fmt.Printf("goAtoi: [%d] \n", i)
	return int(i)
}
//export gLog
func gLog(c *C.char) {
	fmt.Printf("gLog: [%v] \n", C.GoString(c))
}

//export gLogObj
func gLogObj(jvmti *C.jvmtiEnv, env *C.JNIEnv, thread C.jthread, object C.jobject, class C.jclass, size C.jlong) {
	//o := Cjobject(object)
	fmt.Printf("gLogObj: thread=[%v], object=[%v], class=[%v], size=[%v] \n", thread,object,class,size)
}

func main() {}