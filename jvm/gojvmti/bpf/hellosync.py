#!/usr/bin/python

import sys
from bcc import BPF

# This may not work for 4.17 on x64, you need replace kprobe__sys_clone with kprobe____x64_sys_clone
BPF(text='int kprobe__sys_sync(void *ctx) { bpf_trace_printk("Hello, sys.sync()\\n"); return 0; }').trace_print()
