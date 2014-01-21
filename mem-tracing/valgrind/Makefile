S=code.c

all: gleipnir

code.out:
	gcc -g $(S) -o code.out

gleipnir: code.out
	/work/02681/ankitg/workspace/valgrind/bin/valgrind --read-var-info=yes --tool=gleipnir ./code.out; 

clean:
	rm -f gleipnir* *.out
