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
var HEAP_INTERVAL="heap_interval"
var DURATION="duration"				//not working, bug?
var LOGFILE="logfile"
var FUNCCOUNT="funccount"
var COUNT_INTERVAL="count_interval"
var LOGNUMBER="lognumber"
var LOGSIZE="logsize"
var BYTECODE="bytecode"

func gConfig(s string) error {
	usage := "Options: heap_interval=1,duration=10,method=HashMap.getNode,logfile=alloc.log,lognumber=128,funccount=getNode,count_interval=1"
	
	//fmt.Printf("|  options: %s\n", s)
	ss := strings.Split(s, ",")
	for _, pair := range ss {
		z := strings.Split(pair, "=")
		//fmt.Printf("|      option: %d   %s\n", len(z), pair)
		if len(z)<2 {
			return errors.New(usage)
		}
		switch k := z[0]; k{
			case HEAP_INTERVAL:
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
			case FUNCCOUNT:
				if z[1] != "ALL" {
					C.cSetFunc( C.CString(z[1]) )
				}
				C.cRegisterFuncCount();
			case COUNT_INTERVAL:
				i,e:=strconv.ParseInt(z[1], 0, 32)
				if e != nil {
					return errors.New(usage)
				}
				C.cSetCountInterval( C.int(i) )
			case BYTECODE:
				C.cSetFunc( C.CString(z[1]) )
				C.cRegisterBytecode();
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
	fmt.Printf("-----------------------------------------------------------\n");
}
