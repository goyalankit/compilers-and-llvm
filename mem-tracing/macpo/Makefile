M=foo
F=bar.c
O=macpo

all: macpo.out

macpo:
	macpo.sh --macpo:gen-trace=$(M) $(F) -o $(O)

macpo.out: macpo
	./$(O)


clean:
	rm -f rose_* *.o *.out $(O)
