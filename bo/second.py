from hyperopt import fmin, hp, tpe, STATUS_OK, Trials, space_eval

def f(p1,p2):
    return {'loss':p1**2+p2**2, 'status':STATUS_OK}


f = lambda space:space['p1']**2 + space['p2']**2
t1 = [100, 80, 60, 40, 20, 10, 0, -10, -20, -40, -60, -80, -100]
t2 = [0, 10, 20, 40, 60, 80, 100]
l = len(t1)*len(t2)
s = {
    'p1':hp.choice('p1', t1),
    'p2': hp.choice('p2', t2),
}
a = tpe.suggest
trials = Trials()
best = fmin(
    fn=f,
    space=s,
    algo=a,
    max_evals=l,
    trials=trials)
print("best id:    ", best)
print("best value: ",space_eval(s,best))
print("trials:")
for t in trials.trials:
    k = t['misc']['vals']
    #v = space_eval(s,k)
    l: int = t['result']['loss']
    #print("v: ", v, "loss: ", l)
    print("key: ", k, "loss: ", l)
    #print(t)

