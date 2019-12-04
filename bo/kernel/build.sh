#gcc -O -DNTIMES=20 -DSTREAM_ARRAY_SIZE=1000000 stream_omp.c -o stream.omp.1M20N
#gcc -O -DNTIMES=8 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M20N

#python2 -m pip install hyperopt
do_cython() {
  echo "cython"
  #cython --embed Optim.py -o tuner.c
  CMD="gcc tuner.c -o tuner.exe $(python-config --cflags)  $(python-config --ldflags) -lz "
  OS=`uname`
  if [ "$OS" == "Darwin" ]; then
    #dynamic
    echo "Mac OSX"
    STATIC=""
    CMD2="$CMD"
  else
    echo "Not Mac OSX"
    CMD2="$CMD -static"
  fi
  #gcc tuner.c -o tuner.exe $(python3-config --cflags) $(python3-config --ldflags)
  echo $CMD2
  $CMD2
}
do_cython
