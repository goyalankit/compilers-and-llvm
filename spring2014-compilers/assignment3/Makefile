all:  live.so

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -g -O0

%.so: %.o 
	$(CXX) -dylib -flat_namespace -shared $^ -o $@
clean:
	rm -f *.o *~ *.so
