export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home
export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk-13.0.1.jdk/Contents/Home
OS=darwin

#clang -shared -undefined dynamic_lookup -o agent.so \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/ \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/darwin/ \
#  agent.cpp 

#clang -shared -undefined dynamic_lookup -o agent.so \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/ \
#  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/darwin/ \
#  agent.cpp 

g++ -O2 -fPIC -shared -I $JAVA_HOME/include -I $JAVA_HOME/include/$OS -olibvmtrace.so vmtrace.cpp
#g++ -O2 -fPIC -shared -I $JAVA_HOME/include -I $JAVA_HOME/include/$OS -olibheapsampler.so heapsampler.cpp
clang -shared -undefined dynamic_lookup -o libheapsampler.so \
  -I $JAVA_HOME/include/ \
  -I $JAVA_HOME/include/darwin/ \
  heapsampler.cpp

clang -shared -undefined dynamic_lookup -o heap_viewer.so \
  -I $JAVA_HOME/include/ \
  -I $JAVA_HOME/include/darwin/ \
  heap_viewer.c
# bytecodes.c

$JAVA_HOME/bin/javac Main.java
