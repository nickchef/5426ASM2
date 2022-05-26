SRC=./src/*.c
INCLUDE_DIR=./include
CC=mpicc
CFLAGS=-std=c11 -O3 -lpthread -o

all:
	${CC} ${SRC} ${CFLAGS} pairwiseComp -I${INCLUDE_DIR}

clean:
	rm ./pairwiseComp
