import timeit as t

TIMES=1
#TIMES = int( sys.argv[1] )
nb = t.timeit('nb.training()', setup='import main_nb as nb', number=TIMES)
print("Numba:  {0:.5f}".format(nb))
cy = t.timeit('cy.training()', setup='import main_cy as cy', number=TIMES)
print("Cython: {0:.5f}".format(cy))
py = t.timeit('py.training()', setup='import main_py as py', number=TIMES)
print("Python: {0:.5f}".format(py))
print("Cython: {0:.5f}".format(cy))
print("Numba:  {0:.5f}".format(nb))
