#gcc -O -DNTIMES=8 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M5N
#gcc -O -DNTIMES=20 -DSTREAM_ARRAY_SIZE=10000000 stream.c -o stream.10M20N

score=`./stream.1M20N |grep Triad |awk '{print $2}'`
echo "{\"score\":$score}" > $TMPDIR/$$.json
#./stream.10M20N |grep Triad |awk '{print $2}'
