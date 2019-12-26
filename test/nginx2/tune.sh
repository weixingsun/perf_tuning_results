tuned-adm profile network-latency
#systemctl stop cpuspeed
#ethtool -C eth0 rx-usecs 0 rx-frames 0 tx-usecs 0 tx-frames 0 pkt-rate-low 0 pkt-rate-high 0
#grub isolcpus,tickless
#isolcpus=1,2,3 nohz_full=1,2,3
#full preemptible kernel(RT)
#https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/
#disable debugging: kernel/memory, stack_overflow_check, 
#make -j28 && sudo make modules_install -j28 && sudo make install -j28
#cd /boot && ls  && sudo update-grub  &&cat grub/grub.cfg

sysctl -w net.ipv4.tcp_rmem="4096 87380 16777216"
sysctl -w net.ipv4.tcp_wmem="4096 65536 16777216"
sysctl -w net.ipv4.tcp_mem="16777216 16777216 16777216"
sysctl -w net.ipv4.tcp_low_latency=1
sysctl -w net.ipv4.tcp_timestamps=0
sysctl -w net.ipv4.tcp_sack=0

sysctl -w net.core.rmem_max=16777216
sysctl -w net.core.wmem_max=16777216
sysctl -w net.core.rmem_default=16777216
sysctl -w net.core.wmem_default=16777216
sysctl -w net.core.optmem_max=16777216
sysctl -w net.core.netdev_max_backlog=250000

for i in "0 1 2"; do
  G=`cat /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor`
  echo "CPU$i.governor=$G"
  echo performance > /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor
done
