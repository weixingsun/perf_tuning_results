ZGC came with JDK11, but we have to enable it explicitly by using flags: -XX:+UnlockExperimentalVMOptions -XX:+UseZGC
This test is aiming to evaluate how good ZGC is performing than the normal throughput parellel GC(PGC)
Note: ZGC Official site published an article about how ZGC compared to G1, so here G1 is ignored
another reason is that G1 has a limitation of 500G memory, it's always not the first choice of production env.
CMS is a very mature GC algo, but due to the memory fraction problem, I cannot introduce a load balancer, so also ignored.

Hardware: 2x Skylake Xeon 8280, 24x 2933MHz 16GB
OS: SLES 15 with kernel 4.12.14-23
Software: SPECjbb2015 kit (composite mode)
Notes: the only difference is the GC option:
ZGC: -XX:+UnlockExperimentalVMOptions -XX:+UseZGC
PGC: -XX:+UseParallelOldGC
HeapSize: 300G
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Results:
          Throughput           Latency*
ZGC       141087(86.5%)        132972(81.5%)
PGC       163133(100%)         137533(84.3%)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Notes: jbb workload calculate geometric mean of 5 SLAs (10ms, 25ms, 50ms, 75ms, 100ms)
which representated as the max transaction response time(latency)


+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ZGC:
(AVG of 5 dumps) Total 1615 threads: 736 blocked, 678 in_native, 201 runnable
CPU Utilization at first load error: us 84% sy 1% id 15%
Avg Pause GC Time 	3.41 ms
Max Pause GC Time 	6.77 ms
GC times: 619, Throughtput: 99.916%
GC Total created bytes 	71.01 tb
GC Avg creation rate 	9.63 gb/sec

GC Pause Total Time 	6 sec 331 ms
GC Pause Avg Time 	3.41 ms
GC Pause Std Dev Time 	0.516 ms
GC Pause Min Time 	1.24 ms
GC Pause Max Time 	6.77 ms

GC Concurrent Total Time 	21 min 46 sec 177 ms
GC Concurrent Avg Time 	2 sec 110 ms
GC Concurrent Std Dev Time 	1 sec 26 ms
GC Concurrent Min Time 	12.5 ms
GC Concurrent Max Time 	4 sec 458 ms
--------------------------
PGC:
(AVG of 5 dumps) Total 1609 threads: 910 blocked, 678 in_native, 21 runnable
CPU Utilization at first load error: us 64% sy 1% id 35%
Avg Pause GC Time 	123 ms
Max Pause GC Time 	280 ms
GC times: 448(minor),  Throughtput: 99.302%
GC Total created bytes 	128.1 tb
GC Total promoted bytes 	6.67 gb
GC Avg creation rate 	16.2 gb/sec
GC Avg promotion rate 	863 kb/sec
Minor GC reclaimed 	128.1 tb
Minor GC total time	55 sec
Minor GC avg time 	123 ms
Minor GC avg time std dev	26.1 ms
Minor GC min/max time	0 / 280 ms
Minor GC Interval avg 	18 sec 113 ms 
Pause Count 	459
Pause total time	56 sec 540 ms
Pause avg time 	123 ms
Pause avg time std dev	0.0
Pause min/max time	0 / 280 ms 

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GC log info shows when being at very high load >70% IR, ZGC not very efficiently utilize the heap 88G->70G, 
but pretty good at beginning <50% IR, 280G->20G

A very interesting finding of heap histo of PGC:
 num     #instances         #bytes  class name (module)
-------------------------------------------------------
   1:       8013854    62737191744  [I (java.base@11.0.2)
   2:     112064403    11599118056  [B (java.base@11.0.2)
   3:     390372430     9368938320  java.lang.Long (java.base@11.0.2)
   4:     159867311     7673630928  java.util.HashMap$Node (java.base@11.0.2)
   5:      64405643     6497135224  [Ljava.lang.Object; (java.base@11.0.2)
   6:       9899012     5302372112  [Ljava.util.HashMap$Node; (java.base@11.0.2)
   7:      89805666     4310671968  java.math.BigDecimal (java.base@11.0.2)
   8:      97462467     3118798944  org.spec.jbb.*
   9:      12967394     2386000496  java.io.ObjectStreamClass (java.base@11.0.2)
  10:      11483321     1529513720  [C (java.base@11.0.2)
  11:      40543774     1297400768  java.lang.String (java.base@11.0.2)
  12:      19669606     1101497936  org.spec.jbb.*
  13:      22465031     1078321488  org.spec.jbb.*
  14:      43271160     1038507840  java.lang.Integer (java.base@11.0.2)
  15:      22243211      889728440  java.util.ArrayList (java.base@11.0.2)
Meanwhile, ZGC only has very limited histo info:
 num     #instances         #bytes  class name (module)
-------------------------------------------------------
   1:       6914261      331884528  java.util.HashMap$Node (java.base@11.0.2)
   2:       9020042      216481008  java.lang.Long (java.base@11.0.2)
   3:       1083123      174968992  [B (java.base@11.0.2)
   4:       3001671      144080208  org.spec.jbb.*
   5:        398602      134100128  [Ljava.util.HashMap$Node; (java.base@11.0.2)
   6:        736256       99377872  [Ljava.lang.Object; (java.base@11.0.2)
   7:       1803593       86572464  java.util.concurrent.ConcurrentHashMap$Node (java.base@11.0.2)
   8:       1740799       69631960  org.spec.jbb.*
   9:         85209       44530616  [Ljava.util.concurrent.ConcurrentHashMap$Node; (java.base@11.0.2)
  10:       1079936       34557952  java.lang.String (java.base@11.0.2)
  11:        605966       29086368  java.math.BigDecimal (java.base@11.0.2)
  12:        579686       27824928  java.util.concurrent.locks.ReentrantLock$NonfairSync (java.base@11.0.2)
  13:        404588       25893632  java.util.HashMap (java.base@11.0.2)
  14:           319       20913640  [Ljava.util.concurrent.ForkJoinTask; (java.base@11.0.2)
  15:        189485       19706440  java.util.concurrent.ConcurrentHashMap (java.base@11.0.2)
  
  take HashMap as an example, PGC has 7G but ZGC only has 300M, will take more snapshots and do more research later.
  

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
And the most exciting part of latency chart:
ZGC detailed response time(ns):

PGC detailed latency data(ns):

