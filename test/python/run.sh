#cython3 --embed python1.py -o cython1.c
#cc cython.c -o cython
#gcc $(pkg-config --libs --cflags python3) cython1.c -o cython1
#python setup.py build_ext --inplace > build.log 2>&1 

python main.py all
#PERF_OPTS="cycles,instructions,cache-references,cache-misses"
#perf stat -e $PERF_OPTS numactl --physcpubind=2 --localalloc python main.py python &
#perf stat -e $PERF_OPTS numactl --physcpubind=2 --localalloc python main.py cython &
#perf stat -e $PERF_OPTS numactl --physcpubind=2 --localalloc python main.py cython_typing &

#rm -rf build fib.c fib.cpython*.so

#PID=`pgrep python`
#perf record -F 99 -ag -p $PID -- sleep 20
