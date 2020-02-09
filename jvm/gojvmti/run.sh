rm -rf heap.h heap.so hs_err*.log /tmp/*  2>/dev/null

FILE1=/home/sun/jbb/jdk13
FILE0=/mnt/d/jdk13
if [ -f "$FILE1" ]; then
    export JAVA_HOME=$FILE1
elif [ -f "$FILE0" ]; then
	export JAVA_HOME=$FILE0
fi
echo "JAVA_HOME=$JAVA_HOME"

#$JAVA_HOME/bin/javac Main.java
#java -XX:+PrintCommandLineFlags -XX:+PrintFlagsFinal > java.flags.txt

FILE1=/s1/clang_llvm_9.0.0
FILE0=/mnt/d/clang_llvm_9.0.0
if [ -f "$FILE1" ]; then
    export LLVM_HOME=$FILE1
elif [ -f "$FILE0" ]; then
	export LLVM_HOME=$FILE0
fi
echo "LLVM_HOME=$LLVM_HOME"
CC=$LLVM_HOME/bin/clang

BCC_HOME=/usr/share/bcc

OS=`uname`
if [ "$OS" == "Linux" ]; then
  OS=linux
else
  OS=darwin
fi
JAVA_INC="-I$JAVA_HOME/include -I$JAVA_HOME/include/$OS"

heapsampler(){
  AGENT=heapsampler.so
  OPTS="-O3 -fPIC $JAVA_INC -lstdc++ -shared"
  $CPP $OPTS -o $AGENT heapsampler_old.cpp
}
heapviewer(){
  AGENT=heapviewer.so
  rm -f $AGENT
  OPTS="-O3 -fPIC $JAVA_INC -lstdc++ -shared"
  $CC $OPTS -o $AGENT heapViewer.c
}
go_build(){
  AGENT=heap.so
  #echo "CC=$CC CXX=$CPP CGO_CFLAGS='$OPTS' go build -buildmode=c-shared -o $AGENT ."
  #CC="$CC" CXX="$CPP" CGO_CFLAGS="$OPTS" go build -buildmode=c-shared -o $AGENT .
  GO_FLAG="-buildmode=c-shared" # -xc -Ofast
  OPTS="-O3 -Ofast -march=native"  # -fPIC -lstdc++ -shared -xc -std=gnu11 -O3 -g -Wall -pedantic -Wextra -Werror
  CC="$CC" CGO_CFLAGS="$OPTS $JAVA_INC" go build $GO_FLAG -o $AGENT ./src
}

flame(){
    #$BCC_HOME/tools/tplist -p $PID '*method*'
    #$BCC_HOME/tools/argdist -p $PID -C "u:$JAVA_HOME/lib/server/libjvm.so:method__entry():char*:arg4" -T 2
    PID=`pgrep java|tail -1`
    examples/perf-map-agent/bin/create-java-perf-map.sh $PID
    sleep 1
    python method.py -p $PID -f 5 > profile.out
    /home/sun/jbb/FlameGraph/flamegraph.pl profile.out > jvm.svg
}
#javac -cp $JAVA_HOME/lib/tools.jar Attacher.java
AGENT=heap.so
LOOP=2000000
run(){
	echo "run without agent:-------------------------------"
    time $JAVA_HOME/bin/java Main $LOOP
}
run_with_agent(){
    AGT=$1
    OPT=$2
    JIT="-XX:+UseParallelOldGC -XX:ParallelGCThreads=1 -XX:+DTraceMethodProbes -XX:+PreserveFramePointer -XX:CompileThreshold=10" # -XX:CompileOnly=Main::count,java.util.HashMap::getNode" #-Xcomp -XX:+DTraceMethodProbes
    #-XX:+EnableJVMCI -XX:+UseJVMCICompiler -XX:-TieredCompilation -XX:+PrintCompilation -XX:+UnlockExperimentalVMOptions 
    echo "$JAVA_HOME/bin/java $JIT -agentpath:./$AGT=$OPT Main $LOOP"
    $JAVA_HOME/bin/java $JIT -agentpath:./$AGT=$OPT Main $LOOP
    $(flame)
}
run_and_attach(){
    AGT=$1
    OPT=$2
    time $JAVA_HOME/bin/java Main $LOOP &
    sleep 1
    pid=`pgrep java`
    echo "$JAVA_HOME/bin/jcmd $pid JVMTI.agent_load ./$AGT $OPT"
    #export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
    #$JAVA_HOME/bin/jcmd $pid VM.flags -all |grep manageable
    $JAVA_HOME/bin/jcmd $pid JVMTI.agent_load ./$AGT "\"$OPT\""
    $(flame)
}
echo "build"
go_build
if [ $? == 0 ]; then
    #go test -bench=.
    #run
    ########################################################################## #objsize,stacktrace not implement, duration not working as it is
    #run_with_agent $AGENT "heap_interval=1048576,logfile=alloc.log,lognumber=128"    #log to file, obj count > 128
    #run_and_attach $AGENT "heap_interval=1048576,lognumber=128"
    #run_with_agent $AGENT "heap_interval=1048576,duration=10"
    #run_and_attach $AGENT "heap_interval=1048576"
    ########################################################################## #methodEntry not work when attaching ...............
    #run_with_agent $AGENT "funccount=getNode,count_interval=1"
    ###################################################################
    #run_with_agent $AGENT "bytecode=HashMap.getNode"  #Main.count, HashMap.getNode
    #run_with_agent $AGENT "thread_cpu=ALL,thread_interval=1"
	
    #run_and_attach $AGENT "heap_interval=1048576,logfile=alloc.log,threshold=128,perfmap=1"
	run_with_agent $AGENT "perfmap=1"
    #heap_sample=[interval=1m;method_depth=3;threshold=128],log=alloc.log
    echo done
fi

###################################################################################
#-----------------------------------------------------------
#| Agent Options: map[interval:1048576 logfile:sample.log]
#| Agent HeapSamplingInterval=1048576
#| Agent Log writes to sample.log
#-----------------------------------------------------------
#Final result= 542894464

#real    0m19.429s
#user    0m55.344s
#sys     0m5.922s
#-----------------------------------------------------------
#| Agent Options: map[interval:10485760 logfile:sample.log]
#| Agent HeapSamplingInterval=10485760
#| Agent Log writes to sample.log
#-----------------------------------------------------------
#Final result= 542894464

#real    0m19.257s
#user    0m56.219s
#sys     0m5.281s
#run without agent:-------------------------------
#Final result= 542894464

#real    0m18.630s
#user    0m55.141s
#sys     0m5.469s

#/mnt/d/jdk13/bin/java 
#-server -XX:+UnlockExperimentalVMOptions -XX:+PrintCompilation -XX:+EnableJVMCI -XX:+UseJVMCICompiler -XX:-TieredCompilation -XX:CompileOnly=Main::count
#-agentpath:./heap.so=bytecode=Main.count Main 20000000
