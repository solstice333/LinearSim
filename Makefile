CC=gcc
FLAGS=-g
FILES=BuildArgs.c CloseList.c LinearSim.c 
OBJ=BuildArgs.o CloseList.o LinearSim.o 
OUT=a.out

all: build

build: $(OBJ)
	$(CC) $(FLAGS) $(OBJ)

BuildArgs.o: BuildArgs.c BuildArgs.h
	$(CC) $(FLAGS) -c  BuildArgs.c

CloseList.o: CloseList.c CloseList.h
	$(CC) $(FLAGS) -c  CloseList.c

LinearSim.o: LinearSim.c Report.h CloseList.h BuildArgs.h
	$(CC) $(FLAGS) -c  LinearSim.c

clean:
	rm -rf $(OBJ) $(OUT)

rebuild: clean build
