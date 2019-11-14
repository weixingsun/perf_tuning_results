from hyperopt import fmin, hp, tpe
s = hp.uniform('x', 0, 1)
f = lambda x: x
f = lambda x: (x-1)**2
a = tpe.suggest
l = 100
best = fmin(
    fn=f,
    space=s,
    algo=a,
    max_evals=l)
print(best)
