#should be statically linked since the disk image used by the VM may not support the dynamic libraries needed by your program
all: test exp1 exp2 exp3

test: test.c
	gcc -static -o test test.c

exp1: exp1.c
	gcc -static -o exp1 exp1.c 

exp2: exp2.c
	gcc -static -o exp2 exp2.c

exp3: exp3.c
	gcc -static -o exp3 exp3.c

clean:
	rm -rf test exp1 exp2 exp3

