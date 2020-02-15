package main

import (
	//"bytes"
	"encoding/binary"
	"strings"
	"fmt"
	"os"
	"os/signal"
	"strconv"
	"time"
	//"unsafe"

	bpf "github.com/iovisor/gobpf/bcc"
)

import "C"

const source string = `
#include <uapi/linux/ptrace.h>
#include <uapi/linux/bpf_perf_event.h>
#include <linux/sched.h>
struct key_t {
    u32 pid;
    u64 kernel_ip;
    u64 kernel_ret_ip;
    int user_stack_id;
    int kernel_stack_id;
    char name[TASK_COMM_LEN];
};
BPF_HASH(counts, struct key_t);
BPF_STACK_TRACE(stack_traces, STACK_TRACE_SIZE);

int do_perf_event(struct bpf_perf_event_data *ctx) {
    u64 id = bpf_get_current_pid_tgid();
    u32 tgid = id >> 32;
    u32 pid = id;
    if (pid == 0)
        return 0;
    if (!(tgid == PID))
        return 0;

    struct key_t key = {.pid = tgid};
    bpf_get_current_comm(&key.name, sizeof(key.name));

    // get stacks
    key.user_stack_id = stack_traces.get_stackid(&ctx->regs, BPF_F_USER_STACK);
    key.kernel_stack_id = stack_traces.get_stackid(&ctx->regs, 0);
    if (key.kernel_stack_id >= 0) {
        // populate extras to fix the kernel stack
        u64 ip = PT_REGS_IP(&ctx->regs);
        u64 page_offset;
        // if ip isn't sane, leave key ips as zero for later checking
#if defined(CONFIG_X86_64) && defined(__PAGE_OFFSET_BASE)
        // x64, 4.16, ..., 4.11, etc., but some earlier kernel didn't have it
        page_offset = __PAGE_OFFSET_BASE;
#elif defined(CONFIG_X86_64) && defined(__PAGE_OFFSET_BASE_L4)
        // x64, 4.17, and later
#if defined(CONFIG_DYNAMIC_MEMORY_LAYOUT) && defined(CONFIG_X86_5LEVEL)
        page_offset = __PAGE_OFFSET_BASE_L5;
#else
        page_offset = __PAGE_OFFSET_BASE_L4;
#endif
#else
        // earlier x86_64 kernels, e.g., 4.6, comes here
        // arm64, s390, powerpc, x86_32
        page_offset = PAGE_OFFSET;
#endif
        if (ip > page_offset) {
            key.kernel_ip = ip;
        }
    }
    counts.increment(key);
    return 0;
}
`

func printMap(m *bpf.Module, tname string){
	fmt.Fprintf(os.Stdout, "%v:\n",tname)
	t := bpf.NewTable(m.TableId(tname), m)
	for it := t.Iter(); it.Next(); {
		fmt.Fprintf(os.Stdout, "%v:%v\n", it.Key(), it.Leaf() )
	}
}
func printMapCounts((m *bpf.Module, tname string){
	fmt.Fprintf(os.Stdout, "%v:\n",tname)
	t := bpf.NewTable(m.TableId(tname), m)
	for it := t.Iter(); it.Next(); {
		k:=it.Key()
		v:=binary.LittleEndian.Uint64(it.Leaf())
		fmt.Fprintf(os.Stdout, "pid=%v,ip=%v,ret=%v,kstack=%v,ustack=%v,name=%s   -> %v\n", 
		    k.pid,k.kernel_ip,k.kernel_ret_ip,k.kernel_stack_id, k.user_stack_id, k.name, v )
	}
}

func build_source(source string, pid int) string {
	source1 := strings.Replace(source, "PID", strconv.Itoa(pid), 1)
	return strings.Replace(source1, "STACK_TRACE_SIZE", "16384", 1)
}
func loadFunc(m *Module) int {
	event, err := m.LoadPerfEvent("do_perf_event")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to load do_perf_event: %s\n", err)
		os.Exit(1)
	}
	return event
}
func attach(m *Module,pid int,freq int,cpu int){
	TYPE :=1	//PERF_TYPE_SOFTWARE=1
	COUNTER:=0	//PERF_COUNT_HW_CPU_CYCLES=0
	PERIOD:=0
	//FREQ:=49
	//cpu:=-1
	groupFunc:=-1
	err = m.AttachPerfEvent(TYPE, COUNTER, PERIOD, freq, pid, cpu, groupFunc, event)
	if err != nil {
		fmt.Fprintf(os.Stderr, "%s\n", err)
	}
}
func sample(duration int, pid int){
	code := build_source(pid)
	m := bpf.NewModule(code, []string{})
	defer m.Close()
	event := loadFunc(m)
	attach(m,pid,49,-1)
}
func block(){
	fmt.Println("Tracing perf events ... hit Ctrl-C to end.")
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, os.Interrupt)
	<-sig
}
func main() {
	duration := 5
	pid:=18934

	sample(duration, pid)
	//block()
	
	time.Sleep(duration * time.Second)
	printMapCounts(m, "counts")
	printMap(m, "stack_traces")

}
