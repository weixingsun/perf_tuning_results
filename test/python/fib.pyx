#import timeit, functools

def fibonacci0(n):
    i = 0
    a = 1
    b = 1
    fib = []
    fib.append(a)
    fib.append(b)
    while i < n:
        c = a + b
        fib.append(c)
        a = b
        b = c
        i += 1
    return fib

def fibonacci_cy(n):
    a,b = 0,1
    for _ in range (1,n):
        a,b = b,a+b
    return b

def fibonacci_cy_styping(int n):
    cdef int _
    cdef int a=0, b=1
    for _ in range(1, n):
        a, b = b, a + b
    return b

#NUM=500000
#t1 = timeit.Timer( functools.partial(fibonacci_py, NUM) )
#print(t1.timeit(1))
