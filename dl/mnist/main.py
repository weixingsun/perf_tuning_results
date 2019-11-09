from main_cy import training as training_cy
from main_py import training as training_py
import timeit ,sys
import functools as f

TIMES=1
#TIMES = int( sys.argv[1] )
tc = timeit.Timer( f.partial(training_cy) ).timeit(TIMES)
tp = timeit.Timer( f.partial(training_py) ).timeit(TIMES)
print("Python: {0:.5f}".format(tp))
print("Cython: {0:.5f}".format(tc))
#import timeit

#cy = timeit.timeit('main_cy.main()', setup='import main_cy', number=1)
#py = timeit.timeit('main_py.main()', setup='import main_py', number=1)

#print(cy, py)
#print('cy is {}x faster'.format(py/cy))
