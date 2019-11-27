# cython: language_level=3,, boundscheck=False, optimize.unpack_method_calls=False
from hyperopt import fmin, hp, tpe, STATUS_OK, Trials, space_eval
import csv, time, os, timeit, json

# "UseTransparentHugePages":["+","-"],
# "UseLargePages":["+","-"],
import sys

LOOPS = 2
OS_LINUX = "linux"

def current_ms():
    return int(round(time.time() * 1000))


def start_benchmark(kv):
    ja = gen_options(kv)
    cmd = 'java ' + array_to_str(ja,' ') + ' -version > /dev/null 2>&1 '
    #os.system(cmd)
    #print("start_benchmark: "+cmd)
    return timeit.timeit("os.system('"+cmd+"')", setup="import os", number=LOOPS)


def array_to_str(jo,sep):
    return sep.join(str(e) for e in jo)


def gen_options(kv):
    jo = []
    for k, v in kv.items():
        if v in ['+','-']:
            if k == 'UseLargePages' and sys.platform[:5] == OS_LINUX:
                jo.append("-XX:+UseTransparentHugePages")
            else:
                jo.append("-XX:"+v+k)
        else:
            jo.append("-XX:"+k+"="+v)
    return jo


def get_score():
    return current_ms()


def F(S):
    score = start_benchmark(S)
    l = -score
    ret = {'loss': l, 'status': STATUS_OK}
    return ret


def read_key_values(path):
    with open('jvm_options.json', 'r') as f:
        jo: dict = json.load(f)
    return jo


def jvm_space_build(f):
    kv = read_key_values(f)  # bios options
    #print(kv)
    S = {}
    for k, v in kv.items():
        S[k] = hp.choice(k, v)
    return S


def space_size(S):
    return 50


def opt(s, f):
    t = Trials()
    best = fmin(
        fn=f,
        space=s,
        algo=tpe.suggest,
        max_evals=space_size(s),
        trials=t)
    return best, t


def print_trial(tt):
    print("trials:")
    for t in tt.trials:
        k = t['misc']['vals']
        # v = space_eval(s,k)
        l: int = t['result']['loss']
        # print("v: ", v, "loss: ", l)
        print("key: ", k, "loss: ", l)
        # print(t)


def print_results(s, r):
    print("=================================")
    print("best id:    ", r)
    print("best value: ", space_eval(s, r))


def main2():
    j = "jvm_options.txt"
    S = jvm_space_build(j)
    R, T = opt(S, F)
    #print_trial(T)
    print_results(S, R)

