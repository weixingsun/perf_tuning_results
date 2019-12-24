#!/bin/bash

setup(){
  export RTE_ANS=/s1/dpdk-ans
  DPDK_DIR=$RTE_ANS/dpdk-18.11
  export RTE_SDK=$DPDK_DIR
  export RTE_TARGET=x86_64-native-linuxapp-gcc
  make config T=x86_64-native-linuxapp-gcc
  make install T=x86_64-native-linuxapp-gcc DESTDIR=x86_64-native-linuxapp-gcc
  KMOD_DIR=$DPDK_DIR/x86_64-native-linuxapp-gcc/kmod/
  insmod $KMOD_DIR/igb_uio.ko
  insmod $KMOD_DIR/rte_kni.ko
  TOOLS_DIR=$DPDK_DIR/usertools/
  #python $DPDK_DIR/usertools/dpdk-devbind.py --status
  #ETH=`python $DPDK_DIR/usertools/dpdk-devbind.py --status |grep 10-Gigabit|grep -v Active|awk -F"if=" '{print $2}'|awk '{print $1}'`
  cd $RTE_ANS/ans
  # ans_main.h    #define MAX_TX_BURST 1
  make
  ########### /usr/local/nginx
  cd $RTE_ANS/dpdk-nginx
  ./configure  --with-http_dav_module
  make && make install
}

svc(){
  IP=$1  #10.0.0.2
  #pkill ans
  #--enable-kni --enable-ipsync  --enable-jumbo --max-pkt-len 9001
  #-w = --pci-whitelist
  #-l = --lcore
  #-n #memory channels
  #--
  #-p #port mask
  #--config=(port,queue,lcore)
  MEM="--base-virtaddr=0x2aaa2aa0000"
  PCI="-w 0000:58:00.0 "  #jbb3
  PCI="-w 0000:06:00.0 "  #dr1.wrk
  PCI="-w 0000:06:00.1 "  #dr1.nginx
  CPU1="-l 1 -n 4 -- -p 0x1 --config='(0,0,1)'"
  #CPU2="-l 2 -n 4 -- -p 0x1 --config='(0,0,2)'"
  #CPU1_2="-l 1,2 -n 4 -- -p 0x1 --config='(0,0,3)'"
  CPU="$CPU1"
  echo "starting ans.nginx"
  #PREFIX="--file-prefix=nginx"
  #echo "nohup ans/build/ans $PREFIX $PCI $MEM $CPU > ans.nginx.log 2>&1 &"
  #      nohup ans/build/ans $PREFIX $PCI $MEM $CPU > ans.nginx.log 2>&1 &
  nohup ans/build/ans $PREFIX $PCI $CPU > ans.nginx.log 2>&1 &
  echo "starting 10s"
  sleep 10
  cli/build/anscli $PREFIX "ip addr add $IP/24 dev veth0"
  cli/build/anscli $PREFIX "ip addr show"
  sleep 1
  /usr/local/nginx/sbin/nginx
  sleep 5
}
##
# ans/build/ans -w 0000:06:00.1 -l 1 -n 4 -- -p 0x1 --config='(0,0,1)'
# ans/build/ans --file-prefix=wrks -w 0000:06:00.0 --base-virtaddr=0x2aaa2aa0000 -l 2 -n 4 -- -p 0x1 --config='(0,0,2)'
##
svc 10.10.10.2
SVR_IP="10.10.10.2"
