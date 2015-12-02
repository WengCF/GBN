all: project2

project2: student2A.o student2B.o student_common.o project2.o
	gcc -g student_common.o student2A.o student2B.o project2.o -o project2

student2A.o: project2.h student2A.c
	gcc -g -c student2A.c

student2B.o: project2.h student2B.c
	gcc -g -c student2B.c

student_common.o: project2.h student_common.c
	gcc -g -c student_common.c

project2.o: project2.h project2.c
	gcc -g -c project2.c

clean:
	rm -rf *.o project2