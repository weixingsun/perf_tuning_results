#!/bin/sh
#gcc -shared libjvm.c -o libjvm.so -I$INC_PATH -L$LIB_PATH -lpython3.7m
#python2 -m pip install hyperopt
do_cython() {
  echo "cython"
  cython --embed jvm.py -o jvm.c
  gcc jvm.c -o jvm.exe $(python-config --cflags)  $(python-config --ldflags)
  #gcc bios.c -o bios.exe $(python3-config --cflags) $(python3-config --ldflags)
}

do_pyinstaller() {
  echo "pyinstaller"
  rm -rf __pycache__/ bin build dist libjvm.c libjvm*.so main.spec
  python setup.py build_ext --inplace 
  pyinstaller --hidden-import hyperopt main.py --onefile
}

do_cython
