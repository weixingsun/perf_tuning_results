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
var INTERVAL="heap_interval"
var DURATION="duration"
var LOGFILE="logfile"
var METHOD="method"
var LOGNUMBER="lognumber"
var LOGSIZE="logsize"

func gConfig(s string) error {
	usage := "Options: heap_interval=1,duration=10,method=HashMap.getNode,logfile=alloc.log,lognumber=128"
	
	//fmt.Printf("|  options: %s\n", s)
	ss := strings.Split(s, ",")
	for _, pair := range ss {
		z := strings.Split(pair, "=")
		//fmt.Printf("|      option: %d   %s\n", len(z), pair)
		if len(z)<2 {
			return errors.New(usage)
		}
		switch k := z[0]; k{
			case INTERVAL:
				i,e:=strconv.ParseInt(z[1], 0, 32)
				if e != nil {
					return errors.New(usage)
				}
				C.cRegisterSampleAlloc();
				C.cSetHeapSamplingInterval( C.int(i) )
			case DURATION:
				i,e:=strconv.ParseInt(z[1], 0, 32)
				if e != nil {
					return errors.New(usage)
				}
				C.cSetDuration( C.int(i) )
			case LOGFILE:
				logpath := z[1]
				C.cSetLogFile( C.CString(logpath) )
			case LOGNUMBER:
				n,e:=strconv.ParseInt(z[1], 0, 32)
				if e != nil {
					return errors.New(usage)
				}
				C.cSetLogNumber( C.int(n) )
			default:
				fmt.Printf("Unsupport option: %s", k);
		}
	}
	return nil
}

//export gOptions
func gOptions(co *C.char) {
	fmt.Printf("-----------------------------------------------------------\n");
	gConfig(C.GoString(co))
}
