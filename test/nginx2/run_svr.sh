#!/bin/bash

setup(){
  export RTE_ANS=/s1/dpdk-ans
  DPDK_DIR=$RTE_ANS/dpdk-18.11
  export RTE_SDK=$DPDK_DIR
  export RTE_TARGET=x86_64-native-linuxapp-gcc
  make config T=x86_64-native-linuxapp-gcc
  make install T=x86_64-native-linuxapp-gcc
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
#ans_svc nginx 1 "0000:06:00.1" 
#ans_svc wrk   2 "0000:06:00.0" 
ans_svc(){
  #--enable-kni --enable-ipsync  --enable-jumbo --max-pkt-len 9001
  #-w = --pci-whitelist
  #-l = --lcore
  #-n #memory channels
  #--
  #-p #port mask
  #--config=(port,queue,lcore)
  MEM="--base-virtaddr=0x2aaa2aa0000"
  NAME="$1"
  PREFIX="--file-prefix=$NAME"
  C=$2
  PCI="-w $3"
  CPU="-l $C -n 4 -- -p 0x1 --config='(0,0,$C)'"
  MEM="--base-virtaddr=0x2aaa2aa0000"
  ANS="ans/build/ans"
  echo "starting $NAME"
  if [ "$NAME" == "nginx" ]; then
    nohup $ANS $PCI $CPU > ans.$NAME.log 2>&1 &
  else
    nohup $ANS $PREFIX $PCI $MEM $CPU > ans.$NAME.log 2>&1 &
  fi
  sleep 10
}
nginx(){
  CMD="/usr/local/nginx/sbin/nginx"
  nohup $CMD > nginx.log 2>&1 &
  sleep 2
}

add_ip(){
  NAME=$1
  IP=$2
  echo "add IP for $NAME: $IP"
  CLI="cli/build/anscli"
  if [ "$NAME" == "nginx" ]; then
    $CLI "ip addr add $IP/24 dev veth0"
    $CLI "ip addr show"
  else
    $CLI $PREFIX "ip addr add $IP/24 dev veth0"
    $CLI $PREFIX "ip addr show"
  fi
  sleep 1
}

stops(){
  NAME=$1
  echo "stopping $NAME services"
  pkill $NAME
  ps -ef|grep $NAME|awk '{print $2}'
}
stops ans
stops nginx
add_ip nginx 10.10.10.2
add_ip wrk 10.10.10.3
ans_svc nginx 1 "0000:06:00.1"
ans_svc wrk   2 "0000:06:00.0"
