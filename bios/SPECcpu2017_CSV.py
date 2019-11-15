from os import listdir
from os.path import isfile, isdir, join
import csv
path = "8280.speedfp"

def list_files(mypath):
    return [f for f in listdir(mypath) if isfile(join(mypath, f))]

def list_named_files(mypath, name):
    return [f for f in listdir(mypath) if isfile(join(mypath, f)) and name in f]

def list_dirs(mypath):
    return [f for f in listdir(mypath) if isdir(join(mypath, f))]

def list_named_lines(file,name):
    results = []
    with open(file,'r') as f:
        for line in f:
            if line.startswith(name):
                results.append(line.rstrip("\n"))
    return results

def get_score(path,dict):
    rsf_file = list_named_files(path,"rsf")[0]
    rsf_full = d2+"/"+rsf_file
    type = rsf_file.split(".")[2]
    score_line = list_named_lines(rsf_full,"spec.cpu2017.basemean:")[0]
    dict["score"]=score_line.split(" ")[1]
    dict["type"]=type
    return dict

def get_bios(path,dict):
    bios_file = list_named_files(d2, "getmydata.all.txt")[0]
    bios_full = d2 + "/" + bios_file
    for line in list_named_lines(bios_full, "Processors"):
        #print(line)  #Processors.SNC=Disable
        data1 = line.split("=")
        data = data1[1]
        title = data1[0].split(".")[1]
        dict[title] = data
    return dict

def WriteDictToCSV(csv_file,dict_data):
    csv_columns = dict_data[0].keys()
    with open(csv_file, 'w') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=csv_columns)
        writer.writeheader()
        for data in dict_data:
            writer.writerow(data)

list = []
for d1 in list_dirs(path):
    line = {}
    d2 = path+"/"+d1

    line = get_score(d2,line)
    line = get_bios(d2,line)
    list.append(line)
    #print(line)

WriteDictToCSV("cpu2017.csv",list)