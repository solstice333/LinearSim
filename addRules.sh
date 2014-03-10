#!/bin/bash

mfName=MakefileStaleyCell

if [ $# -eq 0 ]; then
   echo "Usage: addRules.sh [ file1 file2 ... ]"
   echo "where |file1| is the name of the .c file without the .c extension"
   exit 1
fi

echo "CC=gcc" > $mfName
echo "FLAGS=-g" >> $mfName

echo -ne "FILES=" >> $mfName
for i in $*; do
   echo -ne "$i.c " >> $mfName
done
echo >> $mfName

echo -ne "OBJ=" >> $mfName
for i in $*; do
   echo -ne "$i.o " >> $mfName
done
echo >> $mfName

echo "OUT=a.out" >> $mfName
echo >> $mfName

echo "all: build" >> $mfName
echo >> $mfName

echo 'build: $(OBJ)' >> $mfName 
echo '	$(CC) $(FLAGS) $(OBJ)' >> $mfName
echo >> $mfName

for i in $*; do
   gcc -MM $i.c >> $mfName
   echo '	$(CC) $(FLAGS) -c ' $i.c >> $mfName
   echo >> $mfName
done

echo "clean:" >> $mfName 
echo '	rm -rf $(OBJ) $(OUT)' >> $mfName	
echo >> $mfName

echo "rebuild: clean build" >> $mfName
