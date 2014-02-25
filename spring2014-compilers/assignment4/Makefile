all:  licm.so

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -g -O0

%.so: %.o 
	$(CXX) -dylib -flat_namespace -shared $^ -o $@    

clean:
	rm -f *.o *~ *.so

run: sum-out.bc

sum-simp.bc: sum.bc
	opt -mem2reg sum.bc -o sum-simp.bc

sum.bc: sum.c
	clang -O0 -emit-llvm -c sum.c

sum-out.bc: sum-simp.bc
	opt -stats -load ./licm.so -licm-pass sum-simp.bc -o sum-out.bc

ll: sum-simp.ll sum-out.ll

sum-simp.ll: sum-simp.bc
	llvm-dis sum-simp.bc

sum-out.ll: sum-out.bc
	llvm-dis sum-out.bc


clear:
	rm -f sum-out.bc sum-simp.bc sum-simp.ll sum-out.ll sum.bc

nice: clean clear all run
