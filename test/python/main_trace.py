import timeit, functools
import sys, tracemalloc

def snapshot():
    return tracemalloc.take_snapshot()

def snapshoti_diff(snap1,snap2):
    top_stats = snap2.compare_to(snap1, "lineno")
    print("Top 10 differences:")
    for s in top_stats[:10]:
        print(s)

def fibonacci_py(n):
    snap1 = snapshot()
    a,b = 0,1
    for _ in range (1,n):
        a,b = b,a+b
    snap2 = snapshot()
    snapshoti_diff(snap1,snap2)
    return b

NUM=1000000
TIMES=1

def main():
    if sys.argv[1] == "python":
        tracemalloc.start()
        #tp  = timeit.Timer( functools.partial(fibonacci_py, NUM) ). timeit(TIMES)
        #print("Python: {0:.5f}".format(tp))
        print("length=",len(str(fibonacci_py(NUM))))
    elif sys.argv[1] == "cython":
        import fib  #cython
        tc  = timeit.Timer( functools.partial(fib.fibonacci_cy, NUM) ). timeit(TIMES)
        print("Cython: {0:.5f}".format(tc))
    elif sys.argv[1] == "cython_typing":
        import fib  #cython
        tcs = timeit.Timer( functools.partial(fib.fibonacci_cy_styping, NUM) ). timeit(TIMES)
        print("Cython: {0:.5f}".format(tcs))
    elif sys.argv[1] == "all":
        import fib  #cython
        tp  = timeit.Timer( functools.partial(fibonacci_py, NUM) ). timeit(TIMES)
        tc  = timeit.Timer( functools.partial(fib.fibonacci_cy, NUM) ). timeit(TIMES)
        tcs = timeit.Timer( functools.partial(fib.fibonacci_cy_styping, NUM) ). timeit(TIMES)

        print("Python: {0:.5f} seconds 1.000x".format(tp))
        print("Cython: {1:.5f} seconds {0:.3f}x".format(tp/tc, tc))
        print("Static: {1:.5f} seconds {0:.3f}x".format(tp/tcs, tcs))
    else:
        print("Invalid parameter: "+arg)

if __name__ == "__main__":
    main()
