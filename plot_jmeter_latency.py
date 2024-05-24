#!/usr/bin/python3

import os, sqlite3, argparse, pandas as pd
def exec_cmd(cmd):
    p=subprocess.run([cmd], shell=True, check=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out=p.stdout.decode('utf-8').strip()
    return out
def load_jtl(file):
    df = pd.read_csv(file)
    df[['label','endpoint']] = df['label'].str.split('-',expand=True)
    df=df[['endpoint','Latency']]
    #sum1=df.groupby(['endpoint']).sum()
    #print("sum",sum1)
    count=df.groupby(['endpoint']).count()
    print("count",count)
    mean=df.groupby(['endpoint']).mean()
    print("mean",mean)
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="jmeter latency")
    parser.add_argument("-f", "--file", type=str, default="mgmt_1_no.jtl", help="jtl file name with path")
    parser.add_argument("-n", "--name", type=str, default="order", help="filter name")
    args = parser.parse_args()
    print("latency: ",args.file)
    pd.set_option('display.float_format', lambda x: '%.2f' % x)
    df=load_jtl(args.file)
    #plot_latency(df,args.name)

