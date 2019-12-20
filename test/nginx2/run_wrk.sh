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
  cli/build/anscli "ip addr add 10.20.10.20/24 dev veth0"
  sleep 10
}

wrk(){
  IP=$1
  URL="http://$IP/index.html"
  WRK="dpdk-httpperf/dpdk-httpperf"
  echo "$WRK -t10 -c100 -d20s $URL > client.log 2>&1 &"
  $WRK -t10 -c100 -d20s $URL > client.log 2>&1 &
}
mon(){
  INT="2 8"
  CPU="0-1"
  ssh root@dr1 mpstat -P $CPU $INT > mpstat.server.log 2>&1 &
  mpstat -P $CPU $INT > mpstat.client.log 2>&1 &
  tail -f client.log
}

#svc
wrk 10.20.10.10 
mon
