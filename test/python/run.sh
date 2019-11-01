#cython3 --embed python1.py -o cython1.c
#cc cython.c -o cython
#gcc $(pkg-config --libs --cflags python3) cython1.c -o cython1
python setup.py build_ext --inplace

python main.py

rm fib.c fib.cpython*.so
