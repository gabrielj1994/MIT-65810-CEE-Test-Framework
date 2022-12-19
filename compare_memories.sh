#!/bin/bash

pid_dir1=$1
pid_dir2=$2
filepaths1=("test1" "test2")
filepaths2=("test3" "test4")

# for str1 in ${filepaths1[@]}; do
#   echo $str1
# done

for i in ${!filepaths1[@]}; do
  echo "element $i is ${filepaths1[$i]} ${filepaths2[$i]}"
done

# printf ${filepaths1} | while read path ; do
# printf "\nTesting"
# printf ${path}
# done
exit 0

# today=`date +'%s'`
# hdfs dfs -ls /file/Path/ | grep "^d" | while read line ; do
# dir_date=$(echo ${line} | awk '{print $6}')
# difference=$(( ( ${today} - $(date -d ${dir_date} +%s) ) / ( 24*60*60 ) ))
# filePath=$(echo ${line} | awk '{print $8}')


# if [ ${difference} -gt 10 ]; then
#     hdfs dfs -rm -r $filePath
# fi
# done



