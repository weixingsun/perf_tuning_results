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
)
var INTERVAL="interval"
var LOGFILE="logfile"
func gConfig(s string) (map[string]string, error) {
	ss := strings.Split(s, ",")
	m := make(map[string]string)
	for _, pair := range ss {
		z := strings.Split(pair, "=")
		if len(z)<2 {
			return nil,errors.New("Options: interval=1,logfile=alloc.log")
		}else if z[0] == INTERVAL {
			_,e:=strconv.ParseInt(z[1], 0, 32)
			if e != nil {
				return nil,errors.New("Options: interval=1,logfile=alloc.log")
			}
			m[z[0]] = z[1]
		}else if z[0] == LOGFILE {
			m[z[0]] = z[1]
		}
	}
	return m,nil
}

//export gOptions
func gOptions(jvmti *C.jvmtiEnv, co *C.char) {
	opt := C.GoString(co)
	m,err := gConfig(opt)
	fmt.Printf("-----------------------------------------------------------\n");
	if err != nil {
	    fmt.Printf("| Agent Options Invalid: %v\n", opt)
	}else{
	    fmt.Printf("| Agent Options: %v \n", m)
	}
	i,_:=strconv.ParseInt(m["interval"], 0, 32)
	C.cSetHeapSamplingInterval( jvmti, C.int(i) )
	if f, ok := m["logfile"]; ok {
		C.cSetLogFile( C.CString(f) )
	}
}
