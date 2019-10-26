clang -shared -undefined dynamic_lookup -o agent.so \
  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/ \
  -I /Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home/include/darwin/ \
  agent.cpp 
