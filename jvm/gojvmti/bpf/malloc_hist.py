#!/usr/bin/python
from __future__ import print_function
from bcc import BPF
from bcc.utils import printb
import argparse

def positive_int(val):
    try:
        ival = int(val)
    except ValueError:
        raise argparse.ArgumentTypeError("must be an integer")

    if ival < 0:
        raise argparse.ArgumentTypeError("must be positive")
    return ival


txt = """
#include <uapi/linux/ptrace.h>
BPF_HASH(last);
int trace_malloc(struct pt_regs *ctx){
  u64 ts, *tsp, *cnt, delta, this_cnt, key=0, cnt_key = 1;
  cnt=last.lookup(&cnt_key);
  if (cnt==0) this_cnt=1;
  else {
    this_cnt = ++*cnt;
    last.delete(&cnt_key);
  }
  last.update(&cnt_key, &this_cnt);
  ////////////////////////////////////////////////////
  tsp=last.lookup(&key);
  if(tsp!=0){
    delta = bpf_ktime_get_ns() - *tsp;
    //if(delta<1000000000){
      bpf_trace_printk("%d, %d\\n", this_cnt, delta/1000000);
    //}
    last.delete(&key);
  }
  ts=bpf_ktime_get_ns();
  last.update(&key,&ts);
  return 0;
}

"""
#b.attach_kprobe(event=b.get_syscall_fnname("malloc"), fn_name="do_trace")
#b.attach_uprobe(name="c", sym="malloc", fn_name="trace_malloc", pid=tpid)

#/////////////////////////////////////////////
b = BPF(text=txt)
examples = """examples:
    ./malloccount             # trace libc malloc() bytes until Ctrl-C
    ./malloccount 5           # trace for 5 seconds only
    ./malloccount -f 5        # 5 seconds, and output in folded format
    ./malloccount -m 1000     # only trace I/O more than 1000 usec
    ./malloccount -M 10000    # only trace I/O less than 10000 usec
    ./malloccount -p 185      # only trace threads for PID 185
    ./malloccount -t 188      # only trace thread 188
"""
parser = argparse.ArgumentParser(
    description="Summarize libc malloc() bytes by stack trace",
    formatter_class=argparse.RawDescriptionHelpFormatter,
    epilog=examples)
thread_group = parser.add_mutually_exclusive_group()
thread_group.add_argument("-p", "--pid", metavar="PID", dest="tgid",
    help="trace this PID only", type=positive_int)
args = parser.parse_args()
try:
    tpid = args.pid
except AttributeError:
    tpid = -1

b.attach_uprobe(name="c", sym="malloc", fn_name="trace_malloc", pid=tpid)
#attach_kprobe , attach_kretprobe ,  attach_tracepoint ,  attach_raw_tracepoint ,  attach_xdp, attach_uprobes,  attach_uretprobe
matched = b.num_open_uprobes()
if matched == 0:
    print("error: 0 functions traced. Exiting.", file=stderr)
    exit(1)

###########
print("Tracing for quick malloc ... Ctrl-C to end")
print("Time    PID    COUNT")
while True:
    try:
        (task,pid,cpu,flags,ts,ss) = b.trace_fields()
        printb(b"%.2f : %d  %s" % (ts,pid,ss))
    except KeyboardInterrupt:
        exit()
