import pandas as pd
import plotly.graph_objects as go
import flask,yaml,os,sys,datetime,subprocess,time,argparse,hashlib

def exec_cmd(cmd):
    p = subprocess.run([cmd], shell=True, check=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out = p.stdout.decode("utf-8").strip()
    return out

layout = go.Layout(
    autosize=False,
    width=1500,
    height=500
)

def plot_blaster_cli(file1):
    exec_cmd("echo 'min avg max' > df.csv")
    cmd="grep avg {} |awk -F min '{{print $2}}' > df.log ".format(file1)
    exec_cmd(cmd)
    exec_cmd("sed -i 's/avg//g' df.log ")
    exec_cmd("sed 's/max//g' df.log >> df.csv ")
    data=pd.read_csv("df.csv",delim_whitespace=True)
    data.insert(0,'index', range(1,1+len(data)))
    line1 = go.Scatter(x=data['index'], y=data['min'], name='min')
    line2 = go.Scatter(x=data['index'], y=data['avg'], name='avg')
    line3 = go.Scatter(x=data['index'], y=data['max'], name='max')
    fig = go.Figure(data=[line1,line2,line3],layout=layout)
    fig.update_layout(title="Blaster-cli latency : "+file1)
    fig.write_html(file1+".html")
    #fig.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot blaster-cli output logs")
    parser.add_argument("-f",  "--file", type=str,  default='blaster-cli.log', help="blaster-cli.log")
    args = parser.parse_args()
    plot_blaster_cli(args.file)
