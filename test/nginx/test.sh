#/usr/local/nginx_fstack/sbin/nginx -s stop
#set dpdk.lcore_mask=2,nginx.worker_processes=1,
#set dpdk.lcore_mask=3,nginx.worker_processes=2,
#/usr/local/nginx_fstack/sbin/nginx
#sleep 5
scp ab.sh dr1:/s1/f-stack/
ssh dr1 /s1/f-stack/ab.sh &
sleep 5
mpstat -P 0-4 2 6 > mpstat.log 2>&1 &
perf record -F 99 -g -C 1 -- sleep 20
#perf record -C 1 -g --all-kernel
#perf top -C 1 -g --demangle-kernel
