SRC=./src/*.c
INCLUDE_DIR=./include
CC=mpicc
CFLAGS=-O3 -lpthread -o

all:
	${CC} ${SRC} ${CFLAGS} pairwiseComp -I${INCLUDE_DIR}

clean:
	rm ./pairwiseComp
