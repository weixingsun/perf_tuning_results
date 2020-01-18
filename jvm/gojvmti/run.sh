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
    OPTS="interval=10485760,stacktrace=1,logfile=sample.log"
    LOOP=20000000
    echo "run with agent options: $OPTS"
    time $JAVA_HOME/bin/java -agentpath:./$AGENT=$OPTS Main $LOOP
    echo "run without agent:"
    time $JAVA_HOME/bin/java Main $LOOP
fi


##############
#time	agent:interval=1M	agent:interval=10M	no_agent
#real	9.685s				9.483s				8.862s/8.995s
##############

############## go:decode_class_name2     c:decode_class_name
#cgo_agent	no_agent	c_agent
#42.446s	36.046s		39.396s
#38.955s	32.991s		36.366s
#38.749s	33.255s		
