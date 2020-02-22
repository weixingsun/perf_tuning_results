package main

import (
	"bytes"
	"encoding/binary"
	//"encoding/hex"
	"strings"
	"flag"
	"fmt"
	"os"
	//"os/signal"
	"strconv"
	"time"
	//"unsafe"

	bpf "github.com/weixingsun/gobpf/bcc"
)

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
//BPF_PERF_OUTPUT(counts);
BPF_HASH(counts, struct key_t);
BPF_STACK_TRACE(stack_traces, STACK_TRACE_SIZE);
int do_perf_event(struct bpf_perf_event_data *ctx) {
    u64 id = bpf_get_current_pid_tgid();
    u32 tgid = id >> 32;
    u32 pid = id;
    if (pid == 0) return 0;
    if (!PID) return 0;
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
type PerfEvent struct {
	Pid           uint32
	Pad           uint32
	KernelIp      uint64
	KernelRetIp   uint64
	KernelStackId int
	UserStackId   int
	Name          [16]byte
}
func printMap(m *bpf.Module){
	//fmt.Fprintf(os.Stdout, "%v:\n",tname)
	c := bpf.NewTable(m.TableId("counts"), m)
	st := bpf.NewTable(m.TableId("stack_traces"), m)
	for it := c.Iter(); it.Next(); {
		buf  := it.Key()
		pid  := int(binary.LittleEndian.Uint32(buf[0:4]))
		kip  := binary.LittleEndian.Uint64(buf[8:16])
		//krip := binary.LittleEndian.Uint64(buf[16:24])
		//usid := int32(binary.LittleEndian.Uint32(buf[20:24]))  //why always 0 ?
		usid := int(binary.LittleEndian.Uint32(buf[24:28]))
		ksid := int32(binary.LittleEndian.Uint32(buf[28:32]))
		cmd:=bytes.NewBuffer( buf[32:] ).String()
		x := binary.LittleEndian.Uint64(it.Leaf())
		fn:=""
		if kip > 0 {
			fn=m.GetDemangleSymbolByAddr(kip,-1)+"[k]"
		}else{
			fn=m.GetDemangleSymbolByAddr(uint64(usid),pid)
			syms:=st.GetStackSymbol(usid,pid)
			fmt.Fprintf(os.Stdout, "syms=%s",syms)
		}
		//fmt.Fprintf(os.Stdout, "buf=%v\n", hex.EncodeToString(buf) )
		fmt.Fprintf(os.Stdout, "pid=%d\t[%v]\t--cmd=%s\tksid=%d\tusid=%x\tkip=%x fn=%s\n", pid, x, cmd, ksid, usid, kip, fn )
	}
}

//func bccSymbolByAddr(addr uint64, pid int) string{

func main() {
	t := flag.Int("time", 5, "sampling time, defaults to 5s")
	p := flag.Int("pid", -1, "pid, defaults to -1")
	flag.Parse()
	duration := *t
	pid := *p
	//fmt.Printf("pid=%d time=%d",pid,duration)
	PID := "1"
	if pid>0 {
		PID="(tgid=="+strconv.Itoa(pid)+")"
	}
	source1 := strings.Replace(source, "PID", PID, 1)
	code := strings.Replace(source1, "STACK_TRACE_SIZE", "16384", 1)
	//fmt.Println(code)

	m := bpf.NewModule(code, []string{})
	defer m.Close()
	event, err := m.LoadPerfEvent("do_perf_event")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to load do_perf_event: %s\n", err)
		os.Exit(1)
	}
	//fmt.Fprintf(os.Stdout, "Loaded do_perf_event\n")

	TYPE :=1	//PERF_TYPE_HARDWARE=0,PERF_TYPE_SOFTWARE=1
	COUNTER:=0	//perf_sw_id: PERF_COUNT_HW_CPU_CYCLES=0 PERF_COUNT_SW_BPF_OUTPUT=10
	PERIOD:=0
	FREQ:=49
	cpu:=-1
	groupFD:=-1
	fd:=event
	pid=-1 //bug ?
	err = m.AttachPerfEvent(TYPE, COUNTER, PERIOD, FREQ, pid, cpu, groupFD, fd)
	if err != nil {
		fmt.Fprintf(os.Stderr, "%s\n", err)
	}
	//fmt.Println("Tracing perf events ... hit Ctrl-C to end.")
	//sig := make(chan os.Signal, 1)
	//signal.Notify(sig, os.Interrupt)
	//<-sig
	time.Sleep(time.Duration(duration) * time.Second)

	printMap(m)
	//printMap(m, "stack_traces")
}
