#java -agentpath:./libagent.so \
#  -Djava.security.policy=./test.policy \
#  -Djava.security.manager=default Main

java -agentpath:./agent.so Main
