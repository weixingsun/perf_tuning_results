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
var DURATION="duration"
var LOGFILE="logfile"
func gConfig(s string) error {
	usage := "Options: interval=1,duration=10,logfile=alloc.log"
	
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
				C.cSetHeapSamplingInterval( C.int(i) )
			case DURATION:
				i,e:=strconv.ParseInt(z[1], 0, 32)
				if e != nil {
					return errors.New(usage)
				}
				C.cSetDuration( C.int(i) )
			case LOGFILE:
				C.cSetLogFile( C.CString(z[1]) )
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
