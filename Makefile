#CPPFLAGS=$(shell sdl2-config --cflags) $(EXTRA_CPPFLAGS)
#LDLIBS=$(shell sdl2-config --libs) -lGLEW $(EXTRA_LDLIBS)
#CPPFLAGS=-L /lib/x86_64-linux-gnu
CC=gcc
LDLIBS=-lGLEW -lglfw -lGL -lm

#EXTRA_LDLIBS?=-lGL
all: main
	./main

main : ball.vert ball.frag box.vert box.frag main.c LinAlg.h
	#gcc -std=c11   main.c  ${LDLIBS}  -o main 
	g++ -std=c++11   main.c  ${LDLIBS}  -o main 

clean:
	rm -f *.o *~   main
.PHONY: all clean


