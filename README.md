# Kahn's Algorithm OpenMP - Parallel Processing Project
Parallel implementation of Kahn's topological sort algorithm using OMP<br>
[CEID](https://www.ceid.upatras.gr/en), University of Patras

## Diagram 📈
![Diagram](https://raw.githubusercontent.com/kounelisagis/OpenMP-Topological-Sorting-Algorithm/master/images/diagram.png?token=AIU2MROI6PTO7Y5XZHLKHAS67TXEO)

## Team 👪
[Apostolos Kontarinis](https://github.com/AposKonti)<br>
[Agisilaos Kounelis](https://github.com/kounelisagis)<br>
[Ioannis Prokopiou](https://github.com/GiannisProkopiou)<br>
[Ioannis Sina](https://github.com/IoannisSina)

### How to Compile & Run 🏃
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
