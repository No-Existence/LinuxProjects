#!/bin/bash
local cnt=0
while :
do
  ./work.sh >>out.txt 2>>error.txt
  if [[ $? -eq 1 ]] 
  then break
  fi
  (( cnt++ ))
done

echo "fail time : $cnt "