package main

import (
	//"bytes"
	//"encoding/binary"
	"strings"
	"fmt"
	"os"
	//"os/signal"
	"strconv"
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
BPF_STACK_TRACE(stack_traces, 2048);  //stack_storage_size=16384

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

func main() {
	//replace PID with current pid
	pid:=18934
	spid:=strconv.Itoa(pid)
	code := strings.Replace(source, "PID", spid, 1)
	m := bpf.NewModule(code, []string{})
	defer m.Close()
	//fnName := bpf.GetSyscallFnName("execve")
	kprobe, err := m.LoadKprobe("do_perf_event")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to load syscall__execve: %s\n", err)
		os.Exit(1)
	}else{
		fmt.Fprintf(os.Stdout, "Loaded do_perf_event\n")
	}

	TYPE :=1//PERF_TYPE_HARDWARE=0,PERF_TYPE_SOFTWARE=1
	COUNTER:=0  //PERF_COUNT_HW_CPU_CYCLES=0
	PERIOD:=0
	FREQ:=99
	//pid:=-1
	//AttachPerfEvent(evType, evConfig int, samplePeriod int, sampleFreq int, pid, cpu, groupFd, fd int) error
	err = m.AttachPerfEvent(TYPE, COUNTER, PERIOD, FREQ, pid, -1, -1, kprobe)

	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to attach perf event: %s\n", err)
	}else{
		fmt.Fprintf(os.Stdout, "attached to perf event\n")
		//perfMap, err := bpf.InitPerfMap(table, channel)
	}
	//perfMap.Start()
	//<-sig
	//perfMap.Stop()
}
