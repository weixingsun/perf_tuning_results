export RTE_ANS=/s1/dpdk-ans
export RTE_SDK=$RTE_ANS/dpdk-18.11
export RTE_TARGET=x86_64-native-linuxapp-gcc

build_fstack(){
  export FF_PATH=/s1/f-stack
  export FF_DPDK=/s1/f-stack/dpdk/x86_64-native-linuxapp-gcc
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
  /usr/local/nginx_fstack/sbin/nginx -s stop
  cd $FF_PATH/lib
  make
  sudo make install
  cd $FF_PATH/app/nginx-1.16.1/
  bash ./configure --prefix=/usr/local/nginx_fstack --with-ff_module --with-debug --with-ld-opt="-ljemalloc"
  make && make install
  sudo make install
  cd $FF_PATH
}
build_dpdk(){
  cd $RTE_SDK
  make config T=x86_64-native-linuxapp-gcc
  make
  rmmod igb_uio
  rmmod rte_kni
  insmod build/kmod/igb_uio.ko 
  insmod build/kmod/rte_kni.ko
  usertools/dpdk-devbind.py --bind=igb_uio 0000:58:00.0
}
build_ans(){
  cd $RTE_ANS/ans
  # ans/ans_main.h #define MAX_TX_BURST=1
  # ans/ans_main.h #define MAX_PKT_BURST=1
  make clean
  make
}
build_nginx(){
  cd $RTE_ANS/dpdk-nginx
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
  make clean
  ./configure --with-debug --prefix=/usr/local/nginx_ans --with-ld-opt="-ljemalloc"
  make
  make install
}
build_wrk(){
  cd $RTE_ANS/dpdk-httpperf
  make clean
  make
}

build_fstack
build_ans
build_wrk
#build_nginx
