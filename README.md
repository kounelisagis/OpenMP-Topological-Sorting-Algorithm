# Parallel Processing - Project

### How to Compile & Run
```
gcc -O3 -Wall -o dager dager.c
./dager 5000

gcc -O3 -Wall -o serial serial.c
./serial datasets/dag.mtx output.txt

gcc -O3 -Wall -fopenmp -o parallel_single parallel_single.c
./parallel_single datasets/dag.mtx output.txt

gcc -O3 -Wall -fopenmp -o parallel_multiple parallel_multiple.c
./parallel_multiple datasets/dag.mtx output.txt

gcc -O3 -Wall -o validator validator.c
./validator datasets/dag.mtx output.txt
```
