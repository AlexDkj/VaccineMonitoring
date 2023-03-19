#!/bin/bash

RANGE=100

FLOOR=0

number=0

filename=""

function numb {
   while [ "$number" -le $FLOOR ]
   do
      number=$RANDOM
      let "number %= $RANGE"
   done
   echo -n $number
}

function answer {
   number=$RANDOM
   let "number %= $RANGE"

   if [ $number -eq 1 ]
    then
       echo -n "YES"
   else if [ $number -eq 0 ]
    then
       echo -n "NO"
   fi
   fi
}

function readfromfile {

  number=0
  n=0

  number=$RANDOM
  let "number %= $RANGE"

  while read line
    do
     if [ $n -eq $number ]; then
        echo -n $line
        echo -n " "
     fi
     let "n=n+1"
    done < $filename

  n=0
  number=0
}

number=0
count=0
num='a'
num2='A'
error='z'
error2='Z'

ercharint=$(printf "%d" "'$error'")

ercharint2=$(printf "%d" "'$error2'")

ercharint3=$(printf "%d" "'$num2'")

charint=$(printf "%d" "'$num'")

for j in `seq 1 $3`
do

  if [ $4 -eq 0 ]
  then
    RANGE=$3
  elif [ $4 -eq 1 ]
  then
    let RANGE=($3-100)
  fi

  FLOOR=5

  numb
  echo -n " "
  for i in `seq 1 2`
  do
    while [ "$count" -le 9 ]
    do
       number=$RANDOM
       let "number %= 25"
       let "count += 1"
       let charint=$charint+number
       newchar=$(printf "\x$(printf %x $charint)")
       if [ $charint -le $ercharint ] && [ $charint -ge $charint ] || [ $charint -le $ercharint2 ] && [ $charint -ge $ercharint3 ]
       then
          echo -n $newchar
       fi
    done
    let charint=$(printf "%d" "'$num'")
    let "count = 0"
    echo -n " "
  done

  filename=$1

  RANGE=6
  FLOOR=0
  number=0

  readfromfile

  RANGE=120
  FLOOR=1
  number=0
  numb
  echo -n " "

  RANGE=6
  FLOOR=0
  number=0

  filename=$2

  readfromfile

  FLOOR=0
  RANGE=2
  number=0
  answer
  echo -n " "

  if [ $number -eq 1 ]
  then

     FLOOR=10
     RANGE=30
     number=0
     numb
     echo -n "-"

     FLOOR=9
     RANGE=12
     number=0
     numb
     echo -n "-"

     FLOOR=2000
     RANGE=2030
     number=0
     numb

  fi
  RANGE=6
  FLOOR=0
  number=0
  echo
done
