#!/bin/bash

#
# Created by astesh on 21.09.22.
#

#Реализуйте программу для командного интерпретатора bash, которая вычисляет вещественные корни системы линейных алгебраических уравнений методом Гаусса.


index=0
ind=0
row=()
mx=()

while read line; do
  IFS=","
  read -a row <<<"$line"
  for item in ${row[*]}; do
    mx[$ind]=$item
    ind=$(($ind + 1))
  done
  index=$(($index + 1))
done <$1

for ((i = 0; i < $index; i++)); do
  div=${mx[$i * ($index + 1) + $i]}
  for ((j = 0; j <= $index; j++)); do
    mx[$i * ($index + 1) + $j]=$(echo ${mx[$i * ($index + 1) + $j]}'/'$div | bc -l)
  done
  for ((k = ($i + 1); k < $index; k++)); do
    mul=${mx[$k * ($index + 1) + $i]}
    for ((j = 0; j <= $index; j++)); do
      mx[$k * ($index + 1) + $j]=$(echo ${mx[$k * ($index + 1) + $j]}'-('${mx[$i * ($index + 1) + $j]}'*'$mul')' | bc -l)
    done
  done
done

for ((i = $index - 1; i > 0; i--)); do
  for ((j = i - 1; j >= 0; j--)); do
    mx[($j + 1) * ($index + 1) - 1]=$(echo ${mx[($j + 1) * ($index + 1) - 1]}'-('${mx[($i) * ($index + 1) + $i]}'*'${mx[($i + 1) * ($index + 1) - 1]}')' | bc -l)
  done
done

for ((i = 0; i < index; i++)); do
  echo ${mx[($i + 1) * ($index + 1) - 1]%.*}
done
