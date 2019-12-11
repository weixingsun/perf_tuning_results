DIR=/s1/f-stack
URL=http://10.20.10.20/logo.svg
NUM=2000000
CONN=100
CPUS=( 1 2 3 4 5 6 7 8 9 10 )
for CPU in "${CPUS[@]}"
do
  nohup numactl --physcpubind=$CPU --localalloc ab -n $NUM -c $CONN $URL > $DIR/logs/ab$CPU.log 2>&1 &
  #CMD="nohup numactl --physcpubind=$CPU --localalloc ab -n $NUM -c $CONN $URL > $DIR/logs/ab$CPU.log 2>&1 &"
  #333333333echo $CMD
  #$CMD
done
mpstat -P 0-10 2 8 > $DIR/logs/mpstat.log &

