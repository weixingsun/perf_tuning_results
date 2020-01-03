export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home
export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk-13.0.1.jdk/Contents/Home
export JAVA_HOME=/home/sun/jdk-13.0.1
export LLVM_HOME=/home/sun/clang_llvm_9.0.0
OS=`uname`
if [ "$OS" == "Linux" ]; then
  OS=linux
else
  OS=darwin
fi

#clang -shared -undefined dynamic_lookup -o agent.so \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/ \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/$OS/ \
#  agent.cpp 

#clang -shared -undefined dynamic_lookup -o agent.so \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/ \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/$OS/ \
#  agent.cpp 

CC=$LLVM_HOME/bin/clang
#CC=gcc
CPP=$LLVM_HOME/bin/clang++
OPTS="-O2 -fPIC -shared -I $JAVA_HOME/include -I $JAVA_HOME/include/$OS"
$CPP $OPTS -o libvmtrace.so     vmtrace.cpp
$CPP $OPTS -o libheapsampler.so heapsampler.cpp
#$CC -shared -undefined dynamic_lookup -o libheapsampler.so -I $JAVA_HOME/include/ -I $JAVA_HOME/include/$OS/ heapsampler.cpp

#$CC -shared -undefined dynamic_lookup -o heap_viewer.so -I $JAVA_HOME/include/ -I $JAVA_HOME/include/$OS/ heap_viewer.c
# bytecodes.c
OPT="-Xlint:deprecation"
$JAVA_HOME/bin/javac $OPT Main.java
