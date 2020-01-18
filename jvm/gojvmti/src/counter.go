package main

import (
	"fmt"
	"sync"
)

type Counter struct {
	mu     sync.Mutex
	values map[string]int32
}

func (s *Counter) Inc(key string) {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.values[key]++
	//return s.values[key]
}
func (s *Counter) GetAllCount() {
	//s.mu.Lock()
	//defer s.mu.Unlock()
	fmt.Printf("Alloc: %v\n", s.values)
}
func (s *Counter) Clean() {
	s.values = make(map[string]int32)
}
func NewCounter() Counter {
	return Counter {
		values: make(map[string]int32),
	}
}
/////////////////////////////////////////////
//"sync/atomic"
//var object_counter map[string]int
//func atomicInc(k string){
//	atomic.AddInt32(&object_counter[k],1)
//}
/////////////////////////////////////////////