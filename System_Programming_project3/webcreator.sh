#!/bin/bash
if [ "$#" -ne 4 ]         #elegxos gia ton arithmo twn args
 then echo lathos args
    exit 1
fi
root_directory=$1
text_file=$2
w=$3
p=$4
#header="<!DOCTYPE html><html><body>"
#footer="</body></html>"
if [ -d $1 ] #check if directory exists
 then
  echo to $1  directory exists
fi
if [ -e $2 ]  #check if the txt file exists
  then if [ -f $2 ]
    then echo regular file
  fi
fi
if [ $3 -eq $3 2> /dev/null ]
 then
   echo "integer"
else
  echo "fourth arg is not integer please enter an integer"
  exit 1
fi
if [ $4 -eq $4 2> /dev/null ]
 then
   echo "integer"
else
  echo "fifth arg is not integer please enter an integer"
  exit 1
fi
temp5=`ls $root_directory`
if (( `expr length "temp5"` > 0 ))
 then
   echo Warning:directory is full,purging
   rm -rf $root_directory/*
fi
lines=$(wc -l <"$text_file")
#echo "$lines"
if [ "$lines" -lt 10000 ]
 then
  echo the number of lines is small
  exit 2
fi
let total_pages=w*p
for ((i=0;i<$total_pages;i++))
 do
   yes[$i]=0
 done
position=0
for ((i=0;i<$w;i++))
 do

  mkdir "$root_directory/site$i"
   for ((j=0;j<$p;j++))
    do
     array[$position]=$RANDOM
     array1[$position]=${array[$position]}
     #touch "$root_directory/site$i/page$j""_${array[$position]}.html"
     file[$position]="$root_directory/site$i/page$i""_${array[$position]}.html"
     file1[$position]="/site$i/page$i""_${array1[$position]}.html"
     #file[$position]="/site$i/page$i""_${array[$position]}.html"
     let "position = position + 1"
   done
 done
 position=0
 counter=0
 for ((i=0;i<$w;i++))
  do
    echo Creating web site $i
    for ((j=0;j<$p;j++))
     do
       filename=${file[$position]}
       let temp=lines-2002
       k=$(($RANDOM % $temp +2))
       m=$(($RANDOM % 999 + 1001))
       let f=p/2+1
       let q=w/2+1
       if [ "$p" -eq 1 ]
        then
         f=0
         let "counter=counter+1"
       fi
       for ((r=0;r<$f;r++))
        do
         let first=i*p
         let current=i*p+j
         inlink[$r]=$(($RANDOM % $p +$first))
         while [ "${inlink[$r]}" -eq "$current" ]
          do
           inlink[$r]=$(($RANDOM % $p + $first))
          done
          let "yes[$position]=yes[$position]+1"
        done
        if [ "$w" -eq 1 ]
         then
          q=0
          let "counter=counter+1"
        fi
       for ((l=0;l<$q;l++))
        do
         let temp1=w-1
         let external=temp1*p
         let first=i*p
         let last=first+p-1
         external_link[$l]=$(($RANDOM % $external))
         if [ "${external_link[$l]}" -ge "$first" -a "${external_link[$l]}" -le "$last" ]
          then
           let temp2=w-1-i
           let "external_link[$l]=external_link[$l]+p*temp2"
         fi
        done
        if [ "$counter" -eq 2 ]
         then
          f=1
          q=1
          let temp9=f+q
          let copy=m/temp9
          echo Creating page $filename with $m lines at line $k
        else
          let temp3=f+q
          let copy=m/temp3
          copy1=$copy
          if [ "$j" -eq 0 ]
           then
             echo Creating page $filename with $m lines at line $k
          else
            echo Adding link to $filename
          fi
        fi
       echo "<!DOCTYPE html><html><body>" > $filename
       for ((M=0;M<$f;M++))
        do
          if [ "$counter" -eq 2 ]
           then
             let temp100=k+copy
             temp20=`sed -n -e "$k,$temp100 p" -e "$temp100 q" $text_file`
             echo $temp20 >> $filename  #vazw tis grammes sto arxeio
             echo "</body></html>" >> $filename
             exit 1
          fi
           let temp101=k+copy
           temp20=`sed -n -e "$k,$temp101 p" -e "$temp101 q" $text_file`
           echo $temp20 >> $filename  #vazw tis grammes sto arxeio
           temp4=${inlink[$M]}
           echo "<a href=..${file1[$temp4]}>link$M""_text</a>"
           let k=k+1+copy
        done >> $filename
        copyk=$temp101
        for ((M=0;M<$q;M++))
         do
           let temp102=copyk+copy1
           temp500=`sed -n -e "$copyk,$temp102 p" -e "$temp102 q" $text_file`
           echo $temp500 >> $filename  #vazw tis grammes sto arxeio
            temp30=${external_link[$M]}
            echo "<a href=..${file1[$temp30]}>link$M""_text</a>"
            let copyk=copyk+1+copy1
         done >> $filename
         echo "</body></html>" >> $filename
         let " position = position + 1 "
    done
  done
for ((i=0;i<$total_pages;i++))
 do
   if [ "${yes[$i]}" -eq 0 ]
    then
     echo Not all pages have incoming link
     echo Done
     exit 1
   fi
 done
echo All pages have at least one incoming link
echo Done
