echo "python running:"
time python Optim.py --config=java_config.json > py.log
#python Optim.py --config=java_config.json 
echo "cython running:"
time ./tuner.exe --config=java_config.json > cy.log
