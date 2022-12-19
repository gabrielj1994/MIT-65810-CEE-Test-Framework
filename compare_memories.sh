#!/bin/bash

pid_dir1=$1
pid_dir2=$2
filepaths1=("dummy1")
filepaths2=("dummy2")

# for str1 in ${filepaths1[@]}; do
#   echo $str1
# done



# printf ${filepaths1} | while read path ; do
# printf "\nTesting"
# printf ${path}
# done

while read line ; do
echo ${line}
file_path=$(echo ${line} | awk '{print $9}')
echo ${file_path}
filepaths1[${#filepaths1[@]}]=${file_path}
done <<<$(ls -lrtad ${pid_dir1}/* | grep output\d*)

while read line ; do
echo ${line}
file_path=$(echo ${line} | awk '{print $9}')
echo ${file_path}
filepaths2[${#filepaths2[@]}]=${file_path}
done <<<$(ls -lrtad ${pid_dir2}/* | grep output\d*)

for i in ${!filepaths1[@]}; do
  echo "element $i is ${filepaths1[$i]} ${filepaths1[$i]}"
  sudo ./print_map ${filepaths1[$i]} ${filepaths1[$i]}
done


# today=`date +'%s'`
# hdfs dfs -ls /file/Path/ | grep "^d" | while read line ; do
# dir_date=$(echo ${line} | awk '{print $6}')
# difference=$(( ( ${today} - $(date -d ${dir_date} +%s) ) / ( 24*60*60 ) ))
# filePath=$(echo ${line} | awk '{print $9}')


# if [ ${difference} -gt 10 ]; then
#     hdfs dfs -rm -r $filePath
# fi
# done



