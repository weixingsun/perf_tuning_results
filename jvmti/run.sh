export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home
export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk-13.0.1.jdk/Contents/Home
export JAVA_HOME=/home/sun/jdk-13.0.1
OS=darwin
rm -f gc*.log
nohup $JAVA_HOME/bin/java -Xms10m -Xmx50m -Xlog:gc*:file=gc-%p-%t.log -agentpath:./libheapsampler.so Main 200000000 > heapsampler.log 2>&1 &
#######$JAVA_HOME/bin/jcmd `pgrep java` JVMTI.agent_load ./libheapsampler.so 3

#$JAVA_HOME/bin/java -Xms10m -Xmx50m -agentpath:./heap_viewer.so Main
#$JAVA_HOME/bin/java -agentpath:./libvmtrace.so=stdout.vmtrace.log Main 200000000
#jcmd <pid> JVMTI.agent_load ./libheapsampler.so [interval]

#sleep 1
#PID=`pgrep java`
#echo $JAVA_HOME/bin/jmap -histo $PID
#$JAVA_HOME/bin/jmap -histo $PID > jmap.histo.log
#jmap -histo[:live] `pgrep java`
