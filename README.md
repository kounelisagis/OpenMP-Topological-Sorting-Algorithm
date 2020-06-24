# Kahn's Algorithm OpenMP - Parallel Processing Project
Parallel implementation of Kahn's topological sort algorithm using OMP<br>
CEID, University of Patras

## Diagram
![Diagram](https://raw.githubusercontent.com/kounelisagis/OpenMP-Topological-Sorting-Algorithm/master/images/diagram.png)

## Team
Apostolos Kontarinis<br>
Agisilaos Kounelis<br>
John Prokopiou<br>
John Sina

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
