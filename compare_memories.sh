#!/bin/bash

pid_dir1=$1
pid_dir2=$2
filepaths1=()
filepaths2=()

# for str1 in ${filepaths1[@]}; do
#   echo $str1
# done

# for i in ${!filepaths1[@]}; do
#   echo "element $i is ${filepaths1[$i]} ${filepaths2[$i]}"
# done

# printf ${filepaths1} | while read path ; do
# printf "\nTesting"
# printf ${path}
# done

ls -lrta ${pid_dir1} | grep output\d* | while read line ; do
echo ${line}
file_path=$(echo ${line} | awk '{print $9}')
echo ${file_path}
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



