rm -rf heap.h heap.so hs_err*.log /tmp/*  2>/dev/null

export JAVA_HOME=/mnt/d/jdk13
#$JAVA_HOME/bin/javac Main.java
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
OPTS="-O3 -fPIC $JAVA_INC -lstdc++"
#OPTS="$OPTS -shared "
JAVA_INC="-I$JAVA_HOME/include -I$JAVA_HOME/include/$OS"
GO_FLAG="-buildmode=c-shared"
#CGO_CFLAGS=$JAVA_INC go build $GO_FLAG -o bin/agent-go.so ./src
heapsampler(){
  AGENT=heapsampler.so
  $CPP $OPTS -o $AGENT heapsampler_old.cpp
}
heapviewer(){
  AGENT=heapviewer.so
  rm -f $AGENT
  $CC $OPTS -o $AGENT heapViewer.c
}
go_build(){
  AGENT=heap.so
  #echo "CC=$CC CXX=$CPP CGO_CFLAGS='$OPTS' go build -buildmode=c-shared -o $AGENT ."
  #CC="$CC" CXX="$CPP" CGO_CFLAGS="$OPTS" go build -buildmode=c-shared -o $AGENT .
  CC="$CC" CGO_CFLAGS=$JAVA_INC go build $GO_FLAG -o $AGENT ./src
}

echo "build"
#heapviewer
go_build

if [ $? == 0 ]; then
    OPTS="interval=1048576,stacktrace=1,logfile=sample.log"
    #echo "run with options: $OPTS"
    $JAVA_HOME/bin/java -agentpath:./$AGENT=$OPTS Main 2000000
fi
