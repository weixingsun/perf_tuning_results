package main

import (
	"sync"
	"sync/atomic"
	"testing"
)
//go test -bench=.

///////////////////////////////////////////////////////////////////
type CounterRWMutex struct {
	mu     sync.RWMutex
	values map[string]int64
}
func (s *CounterRWMutex) Clean() {
	s.values = make(map[string]int64)
}
func (s *CounterRWMutex) Inc(key string) {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.values[key]++
	//return s.values[key]
}

func NewCounterRWMutex() CounterRWMutex {
	return CounterRWMutex {
		values: make(map[string]int64),
	}
}
///////////////////////////////////////////////////////////////////
type CounterMutex struct {
	mu     sync.Mutex
	values map[string]int64
}
func (s *CounterMutex) Clean() {
	s.values = make(map[string]int64)
}
func (s *CounterMutex) Inc(key string) {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.values[key]++
	//return s.values[key]
}

func NewCounterMutex() CounterMutex {
	return CounterMutex {
		values: make(map[string]int64),
	}
}
///////////////////////////////////////////////////////////////////
var counters = NewCounterMutex()
var countersRW = NewCounterRWMutex()
///////////////////////////////////////////////////////////////////
//go test -bench ^Benchmark_MutexCounter
func Benchmark_RWMutexCounterInc(b *testing.B) {
	for i := 0; i < b.N; i++ {
		countersRW.Inc("key1")
	}
}
func Benchmark_MutexCounterInc(b *testing.B) {
	for i := 0; i < b.N; i++ {
		counters.Inc("key1")
	}
}
func Benchmark_Mutex(b *testing.B) {
	var lock sync.Mutex
	var value int32

	for i := 0; i < b.N; i++ {
		lock.Lock()
		value = 5
		lock.Unlock()
	}
	_ = value
}
func Benchmark_RWMutex(b *testing.B) {
	var lock sync.RWMutex
	var value int32

	for i := 0; i < b.N; i++ {
		lock.Lock()
		value = 5
		lock.Unlock()
	}
	_ = value
}

func Benchmark_Atomic(b *testing.B) {
	var value int32

	for i := 0; i < b.N; i++ {
		atomic.StoreInt32(&value, 5)
	}
	_ = value
}
///////////////////////////////////////////////////////////////////
func Benchmark_Lock_Goroutine(b *testing.B) {
	var lock sync.RWMutex
	var value int32
	value = 5
	wg := new(sync.WaitGroup)

	for i := 0; i < b.N; i++ {
		wg.Add(1)
		go func(wg *sync.WaitGroup) {
			defer wg.Done()
			lock.RLock()
			defer lock.RUnlock()
			_ = value
		}(wg)
	}
	wg.Wait()
}

func Benchmark_Atomic_Goroutine(b *testing.B) {
	var value int32
	atomic.StoreInt32(&value, 5)

	wg := new(sync.WaitGroup)
	for i := 0; i < b.N; i++ {
		wg.Add(1)
		go func(wg *sync.WaitGroup) {
			defer wg.Done()
			atomic.LoadInt32(&value)
		}(wg)
	}
	wg.Wait()
}
///////////////////////////////////////////////////////////////////
// go test -bench .
// goos: linux
// goarch: amd64
// Benchmark_RWMutexCounterInc-4           10306152               120 ns/op
// Benchmark_MutexCounterInc-4             11182315               109 ns/op
// Benchmark_Mutex-4                       41416585                29 ns/op
// Benchmark_RWMutex-4                     19727594                58 ns/op
// Benchmark_Atomic-4                      100000000               12 ns/op
// Benchmark_Lock_Goroutine-4               2096397               578 ns/op
// Benchmark_Atomic_Goroutine-4             2006214               587 ns/op
// PASS
// ok      _/s1/perf_tuning_results/gojvmti/examples     	10.057s
///////////////////////////////////////////////////////////////////