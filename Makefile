CC=gcc
FLAGS=-g
FILES=LinearSim.c 
OBJ=LinearSim.o 
OUT=a.out

all: build

build: $(OBJ)
	$(CC) $(FLAGS) $(OBJ)

LinearSim.o: LinearSim.c
	$(CC) $(FLAGS) -c  LinearSim.c

clean:
	rm -rf $(OBJ) $(OUT)

rebuild: clean build
