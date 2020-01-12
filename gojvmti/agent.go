package main

//#include <jvmti.h>
import "C"
import (
	"fmt"
	"time"
	"strconv"
)

//export gLoad
func gLoad(jvm *C.jvmtiEnv, cOptions *C.char) {
	options := C.GoString(cOptions)
	fmt.Printf("Agent called with options: [%v] \n", options)
	fmt.Printf("Sleeping 2 seconds and killing JVM...")
	time.Sleep(2 * time.Second)
	//C.cagent_DestroyJvm(jvm)
}
//export goAtoi
func goAtoi(c *C.char) int {
	i,_:=strconv.ParseInt(C.GoString(c), 0, 64)
	fmt.Printf("===============================goAtoi: [%d] \n", i)
	return int(i)
}
//export gCount
func gCount(jvmti *C.jvmtiEnv, cState *C.char) {
	state := C.GoString(cState)
	fmt.Printf("gCount: [%v] \n", state)
}

func main() {}

/*
  - Uses C++'s mutex instead of jvmti
  - Tracks also what got garbage collected so you can see a bit what is being GC'd often
*/