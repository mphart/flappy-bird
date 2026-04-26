CCOMP = g++
CFLAGS = -Wall
CLIB = -lm -lncurses
CFILES = main.cpp
CSTD = 

all: build

build:
	$(CCOMP) $(CSTD) $(CFLAGS) $(HFILES) $(CFILES) -o main $(CLIB)

clean:
	rm -f ./main

run: build
	./main

debug: build
	$(CCOMP) -g $(CSTD) $(CFLAGS) $(HFILES) $(CFILES) -o main $(CLIB)
	gdb ./main