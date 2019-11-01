import timeit, functools
import fib  #cython

def fibonacci_py(n):
    a,b = 0,1
    for _ in range (1,n):
        a,b = b,a+b
    return b

NUM=500000

tp  = timeit.Timer( functools.partial(fibonacci_py, NUM) )
tc  = timeit.Timer( functools.partial(fib.fibonacci_cy, NUM) )
tcs = timeit.Timer( functools.partial(fib.fibonacci_cy_styping, NUM) )

print("Python: "+str(tp.timeit(1)))
print("Cython: "+str(tc.timeit(1)))
print("Cython Static: "+str(tcs.timeit(1)))
