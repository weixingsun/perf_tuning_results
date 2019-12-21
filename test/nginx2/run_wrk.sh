#!/bin/bash

setup(){
  BASE_DIR=/s1/dpdk-ans/dpdk-19.11
  KMOD_DIR=$BASE_DIR/x86_64-native-linuxapp-gcc/kmod/
  insmod $KMOD_DIR/igb_uio.ko
  insmod $KMOD_DIR/rte_kni.ko
  TOOLS_DIR=$BASE_DIR/usertools/
  #python $BASE_DIR/usertools/dpdk-devbind.py --status
  #ETH=`python $BASE_DIR/usertools/dpdk-devbind.py --status |grep 10-Gigabit|grep -v Active|awk -F"if=" '{print $2}'|awk '{print $1}'`
}

svc(){
  IP=$1
  pkill ans
  #--enable-kni --enable-ipsync
  #-w = --pci-whitelist
  #-l = --lcore
  #-n #memory channels
  #--
  #-p #port mask
  #--config=(port,queue,lcore)
  MEM="--base-virtaddr=0x2aaa2aa0000"
  PCI="-w 0000:58:00.0 "
  CPU1="-l 1 -n 4  -- -p 0x1 --config='(0,0,1)'"
  CPU1_2="-l 1,2 -n 4 -- -p 0x1 --config='(0,0,3)'"
  CPU="$CPU1"
  echo "sleeping 10s"
  sleep 10
  echo "nohup ans/build/ans $PCI $MEM $CPU > ans.log 2>&1 &"
  nohup ans/build/ans $PCI $MEM $CPU > ans.log 2>&1 &
  echo "starting 10s"
  sleep 10
  cli/build/anscli "ip addr add $IP/24 dev veth0"
  sleep 10
}

mon(){
  INT="2 8"
  CPU="0-1"
  CLT=logs/client.log
  CPU_CLT=logs/mpstat.client.log
  CPU_SVR=logs/mpstat.server.log
  ssh root@dr1 mpstat -P $CPU $INT > $CPU_SVR 2>&1 &
  mpstat -P $CPU $INT > $CPU_CLT 2>&1 &
  RPSa=`tail -f $CLT | grep -m 1 'Req/Sec'|awk '{print $2}'`
  RPSx=`grep 'Req/Sec' $CLT|awk '{print $4}'`
  LAT50=`grep 50% $CLT |awk '{print $2}'`
  LAT75=`grep 75% $CLT |awk '{print $2}'`
  LAT90=`grep 90% $CLT |awk '{print $2}'`
  LAT99=`grep 99% $CLT |awk '{print $2}'`
  LAT50=${LAT50/.00/}
  LAT75=${LAT75/.00/}
  LAT90=${LAT90/.00/}
  LAT99=${LAT99/.00/}
  LATx=`grep Latency $CLT|grep -v Dist |awk '{print $4}'`
  LATx=${LATx/.00/}
  CFG=`cat CFG.log`
  CPUc=`tail -n1 $CPU_CLT|awk '{printf("%d+%d",$3,$5)}'`
  CPUs=`tail -n1 $CPU_SVR|awk '{printf("%d+%d",$3,$5)}'`
  echo "$CFG	$RPSa	$RPSx	$LAT50	$LAT75	$LAT90	$LAT99	$LATx	$CPUc	$CPUs"
}
wrk(){
  IP=$1
  THREADS=$2
  CONN=$3
  TIME=$4
  #RPS=$5
  URL="http://$IP/index.html"
  CLT=logs/client.log
  WRK="dpdk-httpperf/dpdk-httpperf"
  nohup $WRK --latency -t$THREADS -c$CONN -d$TIME $URL > $CLT 2>&1 &
  echo "$THREADS	$CONN	$TIME" > CFG.log

}
mkdir -p logs
rm -rf logs/*
#svc 10.20.10.20
echo "PROC	CONN	TIME	RPSavg	RPSmax	LAT50	LAT75	LAT90	LAT99  	LATmax	CPU_c	CPU_s"
##### loop thread conn time
for t in {1,10,50,100}; do
  for c in {1,10,50,100}; do
    wrk 10.20.10.10 $t $c 10s
    mon
  done
done
