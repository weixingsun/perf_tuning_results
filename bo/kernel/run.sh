#gcc -O -DNTIMES=20 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M20N
#gcc -O -DNTIMES=8 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M5N

python Optim.py --space=bios_options.txt --stress=stream.sh --setkv=setkv.sh
