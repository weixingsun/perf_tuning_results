from hyperopt import fmin, hp, tpe, STATUS_OK, Trials, space_eval
import csv, time

current_ms = lambda: int(round(time.time() * 1000))
def start_benchmark():
    #print("___benchmarking___")
    pass

def change_bios(k,v):
    #print("bios: ",k,"=",v) #skip this
    pass

def ext(kv):
    for k,v in kv.items():
        change_bios(k,v)
    start_benchmark()

def get_score():
    return current_ms()

def F(S):
    ext(S)
    #time.sleep(5)
    score = get_score()
    l = -score
    ret = {'loss': l, 'status':STATUS_OK}
    return ret

def read_key_values(path):
    kv = {}
    with open(path,"rb") as f:
        for line in f:
            temp = line.split(b":")
            k = temp[0].strip(b" ")
            tv = temp[1]
            v = tv.strip(b'\r\n').lstrip(b' ')[1:-1]
            lv = v.split(b",")
            #print(k, ": " ,len(v)," ",v)
            kv[k]= lv
    return kv

def bios_space_build(f):
    kv = read_key_values(f)      #bios options
    #print(kv)
    S = {}
    for k,v in kv.items():
        S[k] = hp.choice(k,v)
    return S

def space_build():
    t1 = [60, 40, 20, 10, 0, -10, -20, -40, -60]
    t2 = [0, 10, 20, 40, 60, 80, 100]
    return {
        'p1':hp.choice('p1', t1),
        'p2': hp.choice('p2', t2),
    }

def space_size():
    return 50

def opt(s,f):
    T = Trials()
    best = fmin(
        fn=f,
        space=s,
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

def main():
    S = space_build()
    R,T = opt(S,F)
    print_trial(T)
    print_results(S,R)

def main2():
    B = "bios_options.txt"
    S = bios_space_build(B)
    R,T = opt(S,F)
    #print_trial(T)
    print_results(S,R)
    
main2()
