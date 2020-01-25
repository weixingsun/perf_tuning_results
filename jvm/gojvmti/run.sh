rm -rf heap.h heap.so hs_err*.log /tmp/*  2>/dev/null

export JAVA_HOME=/mnt/d/jdk13
#$JAVA_HOME/bin/javac Main.java
#java -XX:+PrintCommandLineFlags -XX:+PrintFlagsFinal > java.flags.txt
#$JAVA_HOME/bin/java $OPT Main.java 200000000

export LLVM_HOME=/mnt/d/clang_llvm_9.0.0
CC=$LLVM_HOME/bin/clang
#CPP=$LLVM_HOME/bin/clang++
#CC=gcc
#CPP=g++

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

#javac -cp $JAVA_HOME/lib/tools.jar Attacher.java
AGENT=heap.so
LOOP=200000
run(){
	echo "run without agent:-------------------------------"
    time $JAVA_HOME/bin/java Main $LOOP
}
run_with_agent(){
	AGT=$1
    OPT=$2
	time $JAVA_HOME/bin/java -agentpath:./$AGT=$OPT Main $LOOP
}
run_and_attach(){
	AGT=$1
    OPT=$2
	time $JAVA_HOME/bin/java Main $LOOP &
	pid=`pgrep java`
	echo "$JAVA_HOME/bin/jcmd $pid JVMTI.agent_load ./$AGT $OPT"
	#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	#$JAVA_HOME/bin/jcmd $pid VM.flags -all |grep manageable
	$JAVA_HOME/bin/jcmd $pid JVMTI.agent_load ./$AGT "\"$OPT\""
}
echo "build"
go_build
if [ $? == 0 ]; then
    #go test -bench=.
    #run
    #run_with_agent $AGENT "heap_interval=1048576,stacktrace=1,logfile=sample.log"
	#run_with_agent $AGENT "heap_interval=1048576,duration=10,logfile=alloc.log,lognumber=128"     #logsize=1024,not implement     
	#run_and_attach $AGENT "heap_interval=1048576,lognumber=128"   #method=HashMap.getNode
	#run_and_attach $AGENT "funccount=count" #methodEntry this will not work when attaching ...............
	#run_with_agent $AGENT "funccount=getNode,count_interval=1"
	###################################################################
	#run_with_agent $AGENT "thread_cpu=ALL,thread_interval=1"
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

