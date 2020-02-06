#!/usr/bin/python
from __future__ import print_function
from bcc import BPF
from bcc.utils import printb

txt = """
#include <uapi/linux/ptrace.h>
BPF_HASH(last);
int do_trace(struct pt_regs *ctx){
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
b = BPF(text=txt)
b.attach_kprobe(event=b.get_syscall_fnname("sync"), fn_name="do_trace")
print("Tracing for quick sync < 1s ... Ctrl-C to end")
start=0
while True:
    try:
        (task,pid,cpu,flags,ts,ss) = b.trace_fields()
        if start == 0:
            start=ts
        ts = ts-start
        printb(b"At time %.2f : nmultiple syncs detected, last %s ms ago" % (ts,ss))
    except KeyboardInterrupt:
        exit()
