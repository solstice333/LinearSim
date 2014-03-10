CC=gcc
FLAGS=-g
FILES=LinearSim.c Cell.c
OBJ=LinearSim.o 
OBJ2=Cell.o
OUT=a.out Cell test*k.out

all: build

build: $(OBJ) $(OBJ2)
	$(CC) $(FLAGS) $(OBJ)
	$(CC) $(FLAGS) $(OBJ2) -o Cell

Cell.o: Cell.c Report.h
	$(CC) $(FLAGS) -c  Cell.c

LinearSim.o: LinearSim.c Report.h
	$(CC) $(FLAGS) -c  LinearSim.c

clean:
	rm -rf $(OBJ) $(OUT) $(OBJ2)

rebuild: clean build
