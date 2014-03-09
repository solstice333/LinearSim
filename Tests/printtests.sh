#!/bin/bash

i=1;

for f in $(ls test*.in); do
   echo -e "TEST$i"
   cat "test$i.in"
   echo -e "\n\n"
   (( i++ ))
done
