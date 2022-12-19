#!/bin/bash

# grep rw-p /proc/$1/maps \
grep busy_loop /proc/$1/maps \
| sed -n 's/^\([0-9a-f]*\)-\([0-9a-f]*\) .*$/\1 \2/p' \
| while read start stop; do \
    gdb --batch --pid $1 -ex \
        "dump memory /opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/$1/output$start 0x$start 0x$stop"; \
done