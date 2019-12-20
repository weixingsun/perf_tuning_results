setup(){
  insmod kmod/rte_kni.ko carrier=on
  insmod kmod/igb_uio.ko
  ifconfig ens1f1 down
  dpdk/usertools/dpdk-devbind.py --bind=igb_uio 0000:06:00.1
  rm -rf /usr/local/nginx_fstack/logs/error.log
  grep Huge /proc/meminfo
}

nginx(){
  /usr/local/nginx_fstack/sbin/nginx -s stop
  sleep 5
  /usr/local/nginx_fstack/sbin/nginx
  sleep 20
}

ip(){
  ipaddr=`grep "^addr" config.ini |awk -F= '{print $2}'`
  netmask=`grep "^netmask" config.ini |awk -F= '{print $2}'`
  broadcast=`grep "^broadcast" config.ini |awk -F= '{print $2}'`
  gateway=`grep "^gateway" config.ini |awk -F= '{print $2}'`
  macaddr="90:e2:ba:c5:dc:bc"
  ifconfig veth0 $ipaddr  netmask $netmask  broadcast $broadcast hw ether $macaddr
  route add -net 0.0.0.0 gw $gateway dev veth0
  echo 1 > /sys/class/net/veth0/carrier # if `carrier=on` not set while `insmod rte_kni.ko`
  # route add -net ...  # other route rules
}
nginx
