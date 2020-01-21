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
echo "build"
go_build

if [ $? == 0 ]; then
    #go test -bench=.
    LOOP=20000000
	OPTS="interval=1048576,stacktrace=1,logfile=sample.log"
    echo "run with agent options: $OPTS"
    time $JAVA_HOME/bin/java -agentpath:./$AGENT=$OPTS Main $LOOP > /dev/null
    OPTS="interval=10485760,stacktrace=1,logfile=sample.log"
    echo "run with agent options: $OPTS"
    time $JAVA_HOME/bin/java -agentpath:./$AGENT=$OPTS Main $LOOP > /dev/null
    echo "run without agent:"
    time $JAVA_HOME/bin/java Main $LOOP
fi

###################################################################################
# maybe not so accurate as the program is variable over
###################################################################################
#run with agent options: interval=1048576,stacktrace=1,logfile=sample.log

#real    0m18.183s
#user    0m54.250s
#sys     0m5.594s
#run with agent options: interval=10485760,stacktrace=1,logfile=sample.log

#real    0m18.119s
#user    0m55.109s
#sys     0m5.188s
#run without agent:
#Final result= 542894464

#real    0m18.094s
#user    0m54.469s
#sys     0m5.094s