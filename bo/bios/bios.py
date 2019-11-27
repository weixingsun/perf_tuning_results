# cython: language_level=3, boundscheck=False, optimize.unpack_method_calls=False
from hyperopt import fmin, hp, tpe, STATUS_OK, Trials, space_eval
import csv, time, pprint, json

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
    if path[-4:] == ".txt":
        return read_kv_txt(path)
    elif path[-5:] == ".json":
        return read_kv_json(path)
    else:
        return None

def read_kv_json(path):
    with open('jvm_options.json', 'r') as f:
        jo: dict = json.load(f)
    return jo

def read_kv_txt(path):
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

def space_build(f):
    kv = read_key_values(f)      #bios options
    #print(kv)
    S = {}
    for k,v in kv.items():
        S[k] = hp.choice(k,v)
    return S


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
    #print("best id:    ", R)
    #print("best value: ", space_eval(S,R))
    pp = pprint.PrettyPrinter(indent=4)
    pp.pprint(space_eval(S,R))

def main():
    S = space_build()
    R,T = opt(S,F)
    #print_trial(T)
    print_results(S,R)

def main():
    B = "bios_options.txt"
    S = space_build(B)
    R,T = opt(S,F)
    #print_trial(T)
    print_results(S,R)
    
main()
