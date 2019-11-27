#cython libjvm.pyx --embed
#INC_PATH=/Users/xuan/anaconda3/include/python3.7m/
#LIB_PATH=/Users/xuan/anaconda3/lib/python3.7/config-3.7m-darwin
#gcc -shared libjvm.c -o libjvm.so -I$INC_PATH -L$LIB_PATH -lpython3.7m

#gcc -c libjvm.c -o libjvm.o $(python3-config --cflags) 
#gcc libjvm.o  -o libjvm.so $(python3-config --ldflags)

rm -rf __pycache__/ bin build dist libbios.c libbios*.so main.spec
python setup.py build_ext --inplace 
pyinstaller --hidden-import hyperopt main.py --onefile

#INC_PATH=/usr/include/python3.7
#PID=`pgrep python`
#perf record -F 99 -ag -p $PID -- sleep 20
