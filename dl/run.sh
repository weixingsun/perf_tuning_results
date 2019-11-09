python setup.py build_ext --inplace > build.log 2>&1 
LOOPS=3000
echo "run $LOOPS loops"
python first_py.py $LOOPS 
python first_cy.py $LOOPS 
#PERF_OPTS="cycles,instructions,cache-references,cache-misses"
#perf stat -e $PERF_OPTS numactl --physcpubind=2 --localalloc python main.py python &
#perf stat -e $PERF_OPTS numactl --physcpubind=2 --localalloc python main.py cython &
#perf stat -e $PERF_OPTS numactl --physcpubind=2 --localalloc python main.py cython_typing &

#rm -rf build fib.c fib.cpython*.so

#PID=`pgrep python`
#perf record -F 99 -ag -p $PID -- sleep 20
