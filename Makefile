CC=gcc
FLAGS=-g
FILES=CloseList.c LinearSim.c 
OBJ=CloseList.o LinearSim.o 
OUT=a.out

all: build

build: $(OBJ)
	$(CC) $(FLAGS) $(OBJ)

CloseList.o: CloseList.c CloseList.h
	$(CC) $(FLAGS) -c  CloseList.c

LinearSim.o: LinearSim.c Report.h CloseList.h
	$(CC) $(FLAGS) -c  LinearSim.c

clean:
	rm -rf $(OBJ) $(OUT)

rebuild: clean build
