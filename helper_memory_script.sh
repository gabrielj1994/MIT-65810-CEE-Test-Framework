#!/bin/sh
sudo bash ./copy_proc.sh $1
sudo bash ./pid_memory_dump.sh $1
mkdir ~/dump_test_${2}_${1}
sudo bash ./compare_memories.sh /opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/$2 /opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/$1 ~/dump_test_${2}_${1}
grep 'Total differences:' ~/dump_test_${2}_${1}/*