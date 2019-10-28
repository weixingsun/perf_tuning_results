export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_112.jdk/Contents/Home
export JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk-13.0.1.jdk/Contents/Home
OS=darwin
rm -f gc*.log
$JAVA_HOME/bin/java -Xms10m -Xmx50m -Xlog:gc*:file=gc-%p-%t.log -agentpath:./libheapsampler.so Main 200000000
#######$JAVA_HOME/bin/jcmd `pgrep java` JVMTI.agent_load ./libheapsampler.so 3

#$JAVA_HOME/bin/java -Xms10m -Xmx50m -agentpath:./heap_viewer.so Main
#$JAVA_HOME/bin/java -agentpath:./libvmtrace.so=stdout.vmtrace.log Main
#jcmd <pid> JVMTI.agent_load ./libheapsampler.so [interval]
#jmap -histo `pgrep java`
#jmap -histo[:live] `pgrep java`
