import timeit, functools
from guppy import hpy
import sys

h = hpy()

def fibonacci_py(n):
    a,b = 0,1
    for _ in range (1,n):
        a,b = b,a+b
    return b

NUM=10000
TIMES=1

def main():
    if sys.argv[1] == "python":
        tp  = timeit.Timer( functools.partial(fibonacci_py, NUM) ). timeit(TIMES)
        print("Python: {0:.5f}".format(tp))
        print( h.heap() )
    elif sys.argv[1] == "cython":
        tc  = timeit.Timer( functools.partial(fib.fibonacci_cy, NUM) ). timeit(TIMES)
        print("Cython: {0:.5f}".format(tc))
    elif sys.argv[1] == "cython_typing":
        tcs = timeit.Timer( functools.partial(fib.fibonacci_cy_styping, NUM) ). timeit(TIMES)
        print("Cython: {0:.5f}".format(tcs))
    elif sys.argv[1] == "all":
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
