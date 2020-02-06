#!/usr/bin/python

from __future__ import print_function
from bcc import BPF

txt = """
int hello(void *ctx){
  bpf_trace_printk("hello\\n");
  return 0;
}
"""
b = BPF(text=txt)
b.attach_kprobe(event=b.get_syscall_fnname("clone"), fn_name="hello")
print("%-18s %-16s %-6s %-10s %-6s" % ("TIME(s)", "CMD", "PID", "MESSAGE", "CPU"))

while True:
    try:
        (task, pid, cpu, flags, ts, msg) = b.trace_fields()
    except ValueError:
        continue
    except KeyboardInterrupt:
        exit()
    print("%-18.6f %-16s %-6d %s %-2.2f" % (ts, task, pid, msg, cpu))

output = """
TIME(s)            CMD              PID    MESSAGE
3009923.592945000  b'<...>'         19547  b'hello'
3009926.656195000  b'reboot.sh'     19547  b'hello'
3009929.175798000  b'systemd-udevd' 1019   b'hello'
3009929.176268000  b'systemd-udevd' 1019   b'hello'
3009929.176605000  b'systemd-udevd' 1019   b'hello'
3009929.177071000  b'systemd-udevd' 1019   b'hello'
3009929.177638000  b'systemd-udevd' 1019   b'hello'
3009929.178253000  b'systemd-udevd' 1019   b'hello'
3009929.178601000  b'systemd-udevd' 1019   b'hello'
"""
