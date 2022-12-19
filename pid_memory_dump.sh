#!/bin/bash

#-ex "shell grep r[-w]-p /proc/$pid/maps | sed -n 's/^\([0-9a-f]*\)-\([0-9a-f]*\) .*$/\1 \2/p' | while read start stop; do \
#     gdb --batch --pid $1 -ex \
#         "dump memory /proj/final-project/state/$fn/proc-maps-c$core-p$pid-r$start-$stop.log 0x$start 0x$stop"; \
# done"
# 
#
# grep rw-p /proc/$1/maps \
# grep busy_loop /proc/$1/maps \
grep r[-w]-p /proc/$1/maps \
| sed -n 's/^\([0-9a-f]*\)-\([0-9a-f]*\) .*$/\1 \2/p' \
| while read start stop; do \
    gdb --batch --pid $1 -ex \
        "dump memory /opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/$1/output$start 0x$start 0x$stop"; \
done
