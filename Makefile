CC=gcc
FLAGS=-g
FILES=BuildArgs.c CloseList.c LinearSim.c Cell.c
OBJ=BuildArgs.o CloseList.o LinearSim.o
OBJ2=Cell.o
OUT=a.out test*k.out Cell

all: build

build: $(OBJ) $(OBJ2)
	$(CC) $(FLAGS) $(OBJ)
	$(CC) $(FLAGS) $(OBJ2) -o Cell

Cell.o: Cell.c
	$(CC) $(FLAGS) -c Cell.c

BuildArgs.o: BuildArgs.c BuildArgs.h
	$(CC) $(FLAGS) -c  BuildArgs.c

CloseList.o: CloseList.c CloseList.h
	$(CC) $(FLAGS) -c  CloseList.c

LinearSim.o: LinearSim.c Report.h CloseList.h BuildArgs.h
	$(CC) $(FLAGS) -c  LinearSim.c

clean:
	rm -rf $(OBJ) $(OUT) $(OBJ2)

rebuild: clean build
