import pandas as pd
import argparse, os, time, subprocess
import plotly.graph_objects as go

def exec_cmd(cmd):
    p=subprocess.run([cmd], shell=True, check=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out=p.stdout.decode('utf-8').strip()
    return out

def plot_cpu(file,name):
    #print("name=",name)
    log=name.replace('-','_')+".log"
    exec_cmd("echo 'PID USER      PR  NI    VIRT    RES    SHR S %CPU %MEM     TIME+ COMMAND' > {}".format(log))
    exec_cmd("grep '{}' {} >> {}".format(name,file,log))
    data_cpu = pd.read_csv(log, header=[0], delim_whitespace=True)
    #print(data_cpu)
    data_cpu[["%CPU"]] = data_cpu[["%CPU"]].apply(pd.to_numeric)
    data_cpu.reset_index(drop=True, inplace=True)
    data_cpu.insert(0, 'index', range(1, 1+len(data_cpu)))
    #print(data_cpu)
    #total = go.Scatter(x=data_cpu['index'], y=100-data_cpu['%idle'], name='total')
    thread = go.Scatter(x=data_cpu['index'], y=data_cpu['%CPU'], name=name)
    exec_cmd("rm -rf {}".format(log))
    return thread

def plot_all(file,threads):
    lines=[]
    for name in threads:
        line=plot_cpu(file,name)
        lines.append(line)
    fig = go.Figure(lines)
    fig.update_layout(
            title="threads usage",
            title_font_size=20,
            # xaxis_title="Index",
            # yaxis_title="Performance Trends"
            # font_family="Courier New",
            # font_color="blue",
            # title_font_family="Times New Roman",
            # title_font_color="red",
            #legend_title_font_color="green"
        )
    fig.write_html("threads.html")

def find_all_threads(file):
    names=[]
    str_out=exec_cmd("head -2000 {}|grep mbxprd|awk '{{print $1,$NF}}'|sort|uniq|awk '{{print $2}}' ".format(file))
    return str_out.split("\n")

parser = argparse.ArgumentParser(description="VastAI Perf Test")
parser.add_argument("-f", "--file1", type=str, default="first.log", help="top threads mode log file")
parser.add_argument("-g", "--file2", type=str, default="", help="top threads mode log file")
args=parser.parse_args()
if len(args.file2)>0:
    all="merged.log"
    cmd="paste -d $'\n' {} {} > {}".format(args.file1,args.file2,all)
    exec_cmd(cmd)
    threads=find_all_threads(all)
    plot_all(all,threads)
else:
    threads=find_all_threads(args.file1)
    plot_all(args.file1,threads)
