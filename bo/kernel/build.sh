gcc -O -DNTIMES=20 -DSTREAM_ARRAY_SIZE=1000000 stream_omp.c -o stream.omp.1M20N
gcc -O -DNTIMES=8 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M20N

