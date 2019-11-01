import timeit, functools
import fib  #cython

def fibonacci_py(n):
    a,b = 0,1
    for _ in range (1,n):
        a,b = b,a+b
    return b

NUM=500000
TIMES=1
tp  = timeit.Timer( functools.partial(fibonacci_py, NUM) ). timeit(TIMES)
tc  = timeit.Timer( functools.partial(fib.fibonacci_cy, NUM) ). timeit(TIMES)
tcs = timeit.Timer( functools.partial(fib.fibonacci_cy_styping, NUM) ). timeit(TIMES)

print("Python: 1.000x : {0:.5f}".format(tp))
print("Cython: {0:.3f}x : {1:.5f}".format(tp/tc, tc))
print("Cython Static: {0:.3f}x : {1:.5f}".format(tp/tcs, tcs))
