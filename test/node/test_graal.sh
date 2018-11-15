#!/bin/bash

#GraalVM v10.9.0
NODE="/home/sun/jbb/graalvm/bin/node "
#TRACE_OPT="--trace_deopt --perf-basic-prof" # not available for graalvm
JIT_OPT=" --jvm.XX:CompileThreshold=5" #force to use JIT compiler
GC_OPT="--jvm.Xloggc:node-graal.gc.txt --jvm.XX:+PrintGCDetails" # --jvm.XX:CompileThreshold=5
GC_OPT="$GC_OPT --jvm.Xmx3g --jvm.Xms3g --jvm.Xmn2g --jvm.XX:SurvivorRatio=8"
numactl --physcpubind 2 --localalloc $NODE $TRACE_OPT $GC_OPT application.js &
echo "numactl --physcpubind 2 --localalloc $NODE $TRACE_OPT $GC_OPT application.js &"
echo sleep 10
sleep 10
LOG=time_table

echo "waiting for server..."
sleep 5

echo "benchmarking..."

rm $LOG *.log 2> /dev/null
IP="10.10.1.1"
for i in {1..100}
do
  curl -w '%{time_total}\n' -s -o /dev/null "http://$IP:8080/fib" > $LOG &
  curl -w '%{time_total}\n' -s -o /dev/null "http://$IP:8080/fast" > $LOG &
done


#Just checking if we have serve all request
while true
do
  if [[ $(pgrep curl) ]]; then
    echo "waiting..."
  else

    echo "cleaning.."
    curl -s "http://$IP:8080/sample"

    #cleaning some logs emited by v8
    rm *.log 2> /dev/null

    echo "time elapsed: $(tail $LOG)"

    pgrep node | xargs kill

    break
  fi
  sleep 1
done
