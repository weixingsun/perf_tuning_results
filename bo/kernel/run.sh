CFG_JAVA="java_config.json"
CFG_KERNEL="kernel_config.json"
CFG_BIOS="bios_config.json"

py(){
CFG=$1
echo "python running:"
time python Optim.py --config=$CFG --debug=0  --log=1 > py.log
#python Optim.py --config=java_config.json 
}
cy(){
CFG=$1
#export LD_LIBRARY_PATH=.
echo "cython running:"
time ./tuner.exe --config=$CFG --debug=0  --log=1 > cy.log
}

py $CFG_JAVA
cy $CFG_JAVA
