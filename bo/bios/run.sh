#!/bin/sh
#gcc -shared libjvm.c -o libjvm.so -I$INC_PATH -L$LIB_PATH -lpython3.7m

do_cython() {
  echo "cython"
  cython --embed bios.py -o bios.c
  gcc bios.c -o bios.exe $(python-config --cflags)  $(python-config --ldflags)
  #gcc bios.c -o bios.exe $(python3-config --cflags) $(python3-config --ldflags)
}

do_pyinstaller() {
  echo "pyinstaller"
  rm -rf __pycache__/ bin build dist libbios.c libbios*.so main.spec
  python setup.py build_ext --inplace 
  pyinstaller --hidden-import hyperopt main.py --onefile
}

do_cython
./bios.exe --space=./bios_options.txt --score=./score.sh --stress=./stream.sh
