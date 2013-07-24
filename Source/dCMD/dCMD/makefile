GPP = g++
GCC = gcc

OUTFILE = "dCMD.so"

COMPILE_FLAGS = -c -m32 -O3 -fPIC -w -DLINUX -Wall -I./SDK/amx/

all:
	$(GPP) $(COMPILE_FLAGS) ./SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) main.cpp
	$(GPP) -m32 -O3 -fshort-wchar -shared -o $(OUTFILE) *.o -pthread
	rm -f *.o