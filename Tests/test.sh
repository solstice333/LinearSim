#!/bin/bash

read cells1 < test1.in
IFS=' ' read -a values <<< $cells1
cells1=${values[3]}

echo $cells1
