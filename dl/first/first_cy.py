from first import first_train as train
import timeit ,sys
import functools as f

#TIMES=500
TIMES = int( sys.argv[1] )
tc = timeit.Timer( f.partial(train) ).timeit(TIMES)
print("Cython: {0:.5f}".format(tc))
