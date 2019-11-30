#gcc -O -DNTIMES=20 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M20N
#gcc -O -DNTIMES=8 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M5N

#python Optim.py --config=bios_config.json 
python Optim.py --config=java_config.json 
