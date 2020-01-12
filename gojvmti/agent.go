package main

//#include <jvmti.h>
import "C"
import (
	"fmt"
	"strconv"
)

//export gLoad
func gLoad(jvm *C.jvmtiEnv, co *C.char) {
	opt := C.GoString(co)
	fmt.Printf("Agent options: [%v] \n", opt)
	//time.Sleep(2 * time.Second)
	//C.cagent_DestroyJvm(jvm)
}
//export goAtoi
func goAtoi(c *C.char) int {
	i,_:=strconv.ParseInt(C.GoString(c), 0, 64)
	//fmt.Printf("goAtoi: [%d] \n", i)
	return int(i)
}
//export gLog
func gLog(jvmti *C.jvmtiEnv, c *C.char) {
	s := C.GoString(c)
	fmt.Printf("gLog: [%v] \n", s)
}

func main() {}