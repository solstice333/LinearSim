#!/bin/bash

base=$(ps -A | wc -l)

read cells1 < Tests/test1.in
IFS=' ' read -a values <<< $cells1
cells1=${values[3]}

read cells2 < Tests/test2.in
IFS=' ' read -a values <<< $cells2
cells2=${values[3]}

read cells3 < Tests/test3.in
IFS=' ' read -a values <<< $cells3
cells3=${values[3]}

echo -e "\nTEST1\n"
./saferun -p$(($base+$cells1)) ./a.out < Tests/test1.in | sort -n > test1k.out
diff test1k.out Tests/test1.out

echo -e "\nTEST2\n"
./saferun -p$(($base+$cells2)) ./a.out < Tests/test2.in | sort -n > test2k.out
diff test2k.out Tests/test2.out

echo -e "\nTEST3\n"
./saferun -p$(($base+$cells3)) ./a.out < Tests/test3.in | sort -n > test3k.out
diff test3k.out Tests/test3.out

echo -e "\nChecking process list"
ps -u benson | grep Cell && ps -u benson | grep LinearSim && ps -u benson | grep a.out
