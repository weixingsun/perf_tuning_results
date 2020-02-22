
AGENT=profiler.so
rm -rf $AGENT log
LOOP=4000000
JIT="-XX:+UseParallelOldGC -XX:ParallelGCThreads=1 -XX:+PreserveFramePointer"

cpp_build(){
  BCC=/home/sun/perf_tuning_results/jvm/bcc
  BCC_INC="-I$BCC/src/cc -I$BCC/src/cc/api -I$BCC/src/cc/libbpf/include/uapi"
  CC=clang
  CPP=clang++
  OS=linux
  JAVA_HOME=/home/sun/jbb/jdk13
  JAVA_INC="-I$JAVA_HOME/include -I$JAVA_HOME/include/$OS"
  OPTS="-O3 -fPIC -lbcc -lstdc++ -shared $JAVA_INC $BCC_INC"
  echo "$CPP $OPTS -o $AGENT profiler.cpp"
        $CPP $OPTS -o $AGENT profiler.cpp
}

run_and_attach(){
    AGT=$1
    OPT=$2
    time $JAVA_HOME/bin/java $JIT Main $LOOP &
    sleep 1
    pid=`pgrep java`
    echo "$JAVA_HOME/bin/jcmd $pid JVMTI.agent_load ./$AGT $OPT"
    #export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
    #$JAVA_HOME/bin/jcmd $pid VM.flags -all |grep manageable
    nohup $JAVA_HOME/bin/jcmd $pid JVMTI.agent_load ./$AGT "\"$OPT\"" > jcmd.log 2>&1 &
}

run_with_agent(){
    AGT=$1
    OPT=$2
    #-XX:+EnableJVMCI -XX:+UseJVMCICompiler -XX:-TieredCompilation -XX:+PrintCompilation -XX:+UnlockExperimentalVMOptions 
    echo "$JAVA_HOME/bin/java $JIT -agentpath:./$AGT=$OPT Main $LOOP"
    time $JAVA_HOME/bin/java $JIT -agentpath:./$AGT=$OPT Main $LOOP java.log 2>&1 &
}
cpp_build
if [ $? = 0 ]; then
    echo "build done"
    #run_and_attach $AGENT "flame=1"
    run_with_agent $AGENT "flame=3"
fi
