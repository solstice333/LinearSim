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

read cells4 < Tests/test4.in
IFS=' ' read -a values <<< $cells4
cells4=${values[3]}

read cells5 < Tests/test5.in
IFS=' ' read -a values <<< $cells5
cells5=${values[3]}

read cells6 < Tests/test6.in
IFS=' ' read -a values <<< $cells6
cells6=${values[3]}

cells7=5

read cells8 < Tests/test8.in
IFS=' ' read -a values <<< $cells8
cells8=${values[3]}

read cells9 < Tests/test9.in
IFS=' ' read -a values <<< $cells9
cells9=${values[3]}

read cells10 < Tests/test10.in
IFS=' ' read -a values <<< $cells10
cells10=${values[3]}

echo -e "\nTEST1\n"
./saferun -p$(($base+$cells1)) ./a.out < Tests/test1.in | sort -n > test1k.out
diff test1k.out Tests/test1.out

echo -e "\nTEST2\n"
./saferun -p$(($base+$cells2)) ./a.out < Tests/test2.in | sort -n > test2k.out
diff test2k.out Tests/test2.out

echo -e "\nTEST3\n"
./saferun -p$(($base+$cells3)) ./a.out < Tests/test3.in | sort -n > test3k.out
diff test3k.out Tests/test3.out

echo -e "\nTEST4\n"
./saferun -p$(($base+$cells4)) ./a.out < Tests/test4.in | sort -n > test4k.out
diff test4k.out Tests/test4.out

echo -e "\nTEST5\n"
./saferun -p$(($base+$cells5)) ./a.out < Tests/test5.in | sort -n > test5k.out
diff test5k.out Tests/test5.out

echo -e "\nTEST6\n"
./saferun -p$(($base+$cells6)) ./a.out < Tests/test6.in | sort -n > test6k.out
diff test6k.out Tests/test6.out

echo -e "\nTEST7\n"
./saferun -p$(($base+$cells7)) ./a.out < Tests/test7.in | sort -n > test7k.out
diff test7k.out Tests/test7.out

echo -e "\nTEST8\n"
./saferun -p$(($base+$cells8)) ./a.out < Tests/test8.in | sort -n > test8k.out
diff test8k.out Tests/test8.out

echo -e "\nTEST9\n"
./saferun -p$(($base+$cells9)) ./a.out < Tests/test9.in | sort -n > test9k.out
diff test9k.out Tests/test9.out

echo -e "\nTEST10\n"
./saferun -p$(($base+$cells10)) ./a.out < Tests/test10.in | sort -n > test10k.out
diff test10k.out Tests/test10.out

echo -e "\nChecking process list"
ps -u benson | grep Cell && ps -u benson | grep LinearSim && ps -u benson | grep a.out
