from hyperopt import fmin, hp, tpe, STATUS_OK, Trials, space_eval
import csv

def f(S):
    return {'loss':S["p1"]**2+S["p2"]**2, 'status':STATUS_OK}

def csv_to_list(path):
    l = []
    with open(path, 'rb') as f:
        reader = csv.reader(f)
        l = list(reader)
    return l

def space_build():
    t1 = [100, 80, 60, 40, 20, 10, 0, -10, -20, -40, -60, -80, -100]
    t2 = [0, 10, 20, 40, 60, 80, 100]
    return {
        'p1':hp.choice('p1', t1),
        'p2': hp.choice('p2', t2),
    }

def space_size():
    return 50

def opt(S):
    T = Trials()
    best = fmin(
        fn=f,
        space=S,
        algo=tpe.suggest,
        max_evals=space_size(),
        trials=T)
    return best, T
def print_trial(T):
    print("trials:")
    for t in T.trials:
        k = t['misc']['vals']
        #v = space_eval(s,k)
        l: int = t['result']['loss']
        #print("v: ", v, "loss: ", l)
        print("key: ", k, "loss: ", l)
        #print(t)
def print_results(S,R):
    print("=================================")
    print("best id:    ", R)
    print("best value: ", space_eval(S,R))

S = space_build()
R,T = opt(S)
print_trial(T)
print_results(S,R)
