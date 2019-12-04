#gcc -O -DNTIMES=20 -DSTREAM_ARRAY_SIZE=1000000 stream_omp.c -o stream.omp.1M20N
gcc -O -DNTIMES=8 -DSTREAM_ARRAY_SIZE=1000000 stream.c -o stream.1M20N

#python2 -m pip install hyperopt
#pip install pythran
do_cython() {
  echo "cython"
  SRCP="Optim.py"
  SRCC="tuner.c"
  BIN="tuner.exe"
  CC="gcc"
  cython --embed $SRCP -o $SRCC
  #cython --embed Optim.py -o tuner2.c -c=-DUSE_XSIMD -c=-march=native
  #pythran -DUSE_XSIMD -fopenmp -march=native Optim.py
  OPT="-O3 -march=native -mavx2"
  PY_OPT=" $(python-config --cflags)  $(python-config --ldflags) "
  PY_OPT=" $(python3-config --cflags)  $(python3-config --ldflags) "
  CMD="$CC $OPT $SRCC -o $BIN $PY_OPT -lz "
  OS=`uname`
  if [[ $OS -eq "Darwin" ]]; then
    #dynamic
    STATIC=""
    CMD2="$CMD"
  else
    CMD2=`echo "$CMD -static"|sed -e "s/dynamic/static/g"`
  fi
  #gcc tuner.c -o tuner.exe $(python3-config --cflags) $(python3-config --ldflags)
  echo $CMD2
  $CMD2
}
do_cython
