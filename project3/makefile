.SUFFIXES : .c .o
CC = mpiCC
CFLAGS = -o
CLAFGN = -np
OBJS = project3.o
TARGET = project3

project3.o : project3.c

mpi1:
	mpiCC -o project3 project3.c
	./project3 < input1.txt > my_output1.txt
mpi2:
	mpiCC -o project3 project3.c
	./project3 < input2.txt > my_output2.txt
mpi3:
	mpiCC -o project3 project3.c
	./project3 < input3.txt > my_output3.txt
np:
	mpiCC -o project3 project3.c
	time mpirun -np 64 ./project3 < input3.txt > my_output3.txt
it:
	mpiCC -o project3 project3.c
	mpirun -hostfile hosts.txt -np 64 --map-by node ./project3 < test.txt > test_out.txt
host1:
	mpiCC -o project3 project3.c
	mpirun -hostfile hosts.txt -np 9 --map-by node ./project3 < input1.txt > my_output1.txt
host11:
	mpiCC -o project3 project3.c
	mpirun -hostfile hosts.txt -np 64 --map-by node ./project3 < input1.txt
host2:
	mpiCC -o project3 project3.c
	mpirun -hostfile hosts.txt -np 16 --map-by node ./project3 < input2.txt > my_output2.txt
host22:
	mpiCC -o project3 project3.c
	mpirun -hostfile hosts.txt -np 64 --map-by node ./project3 < input2.txt
host3:
	mpiCC -o project3 project3.c
	mpirun -hostfile hosts.txt -np 16 --map-by node ./project3 < input3.txt > my_output3.txt
host33:
	mpiCC -o project3 project3.c
	mpirun -hostfile hosts.txt -np 64 --map-by node ./project3 < input3.txt

total1:
	mpiCC -o project3 project3.c
	./project3 < input1.txt > my_output1.txt
	mpirun -hostfile hosts.txt -np 4 --map-by node ./project3 < input1.txt > my_output4.txt
	mpirun -hostfile hosts.txt -np 9 --map-by node ./project3 < input1.txt > my_output9.txt
	mpirun -hostfile hosts.txt -np 16 --map-by node ./project3 < input1.txt > my_output16.txt
	mpirun -hostfile hosts.txt -np 25 --map-by node ./project3 < input1.txt > my_output25.txt
	mpirun -hostfile hosts.txt -np 36 --map-by node ./project3 < input1.txt > my_output36.txt
	mpirun -hostfile hosts.txt -np 49 --map-by node ./project3 < input1.txt > my_output49.txt
	mpirun -hostfile hosts.txt -np 64 --map-by node ./project3 < input1.txt > my_output64.txt
	diff -bwi my_output4.txt my_output9.txt
	diff -bwi my_output9.txt my_output16.txt
	diff -bwi my_output16.txt my_output25.txt
	diff -bwi my_output25.txt my_output36.txt
	diff -bwi my_output36.txt my_output49.txt
	diff -bwi my_output49.txt my_output64.txt
	diff -bwi my_output64.txt output1.txt

total2:
	mpiCC -o project3 project3.c
	./project3 < input2.txt > my_output1.txt
	mpirun -hostfile hosts.txt -np 4 --map-by node ./project3 < input2.txt > my_output4.txt
	mpirun -hostfile hosts.txt -np 9 --map-by node ./project3 < input2.txt > my_output9.txt
	mpirun -hostfile hosts.txt -np 16 --map-by node ./project3 < input2.txt > my_output16.txt
	mpirun -hostfile hosts.txt -np 25 --map-by node ./project3 < input2.txt > my_output25.txt
	mpirun -hostfile hosts.txt -np 36 --map-by node ./project3 < input2.txt > my_output36.txt
	mpirun -hostfile hosts.txt -np 49 --map-by node ./project3 < input2.txt > my_output49.txt
	mpirun -hostfile hosts.txt -np 64 --map-by node ./project3 < input2.txt > my_output64.txt
	diff -bwi my_output4.txt my_output9.txt
	diff -bwi my_output9.txt my_output16.txt
	diff -bwi my_output16.txt my_output25.txt
	diff -bwi my_output25.txt my_output36.txt
	diff -bwi my_output36.txt my_output49.txt
	diff -bwi my_output49.txt my_output64.txt
	diff -bwi my_output64.txt output2.txt

total3:
	mpiCC -o project3 project3.c
	./project3 < input3.txt > my_output1.txt
	mpirun -hostfile hosts.txt -np 4 --map-by node ./project3 < input3.txt > my_output4.txt
	mpirun -hostfile hosts.txt -np 9 --map-by node ./project3 < input3.txt > my_output9.txt
	mpirun -hostfile hosts.txt -np 16 --map-by node ./project3 < input3.txt > my_output16.txt
	mpirun -hostfile hosts.txt -np 25 --map-by node ./project3 < input3.txt > my_output25.txt
	mpirun -hostfile hosts.txt -np 36 --map-by node ./project3 < input3.txt > my_output36.txt
	mpirun -hostfile hosts.txt -np 49 --map-by node ./project3 < input3.txt > my_output49.txt
	mpirun -hostfile hosts.txt -np 64 --map-by node ./project3 < input3.txt > my_output64.txt
	diff -bwi my_output4.txt my_output9.txt
	diff -bwi my_output9.txt my_output16.txt
	diff -bwi my_output16.txt my_output25.txt
	diff -bwi my_output25.txt my_output36.txt
	diff -bwi my_output36.txt my_output49.txt
	diff -bwi my_output49.txt my_output64.txt
	diff -bwi my_output64.txt output3.txt

