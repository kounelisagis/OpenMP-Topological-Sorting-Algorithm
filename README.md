# Parallel Processing - Project

### How to Compile & Run
```
gcc -O3 -Wall -o dager dager.c
./dager 5000

gcc -O3 -Wall -o serial serial.c stack.c
./serial datasets/dag.mtx output.txt

gcc -O3 -Wall -fopenmp -o parallel parallel.c stack.c
./parallel datasets/dag.mtx output.txt 4

gcc -O3 -Wall -o validator validator.c stack.c
./validator datasets/dag.mtx output.txt
```
