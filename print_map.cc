// SPDX-License-Identifier: GPL-2.0
/*
 * Seccomp filter example for x86 (32-bit and 64-bit) with BPF macros
 *
 * Copyright (c) 2012 The Chromium OS Authors <chromium-os-dev@chromium.org>
 * Author: Will Drewry <wad@chromium.org>
 *
 * The code may be used by anyone for any purpose,
 * and can serve as a starting point for developing
 * applications using prctl(PR_SET_SECCOMP, 2, ...).
 *
 * This code has been modified as a skeleton for lab3 in 6.5810 at MIT.
 */
#define _GNU_SOURCE
// #define __USE_GNU 1
// #define _GNU_SOURCE 1

// #include <linux/types.h>
// #include <linux/filter.h>
// #include <linux/seccomp.h>
// #include <linux/unistd.h>
// #include <signal.h>
// #include <stdio.h>
// #include <stddef.h>
// #include <fcntl.h>
// #include <string.h>
// #include <stdarg.h>
// #include <stdlib.h>
// #include <sys/prctl.h>
// #include <unistd.h>
// #include <errno.h>

// #include <inttypes.h>
// #include <ftw.h>
// #include <string.h>
// #include <time.h>

// #define syscall_arg(_n) (offsetof(struct seccomp_data, args[_n]))
// #define syscall_nr (offsetof(struct seccomp_data, nr))

// #if defined(__i386__)
// #define REG_RESULT	REG_EAX
// #define REG_SYSCALL	REG_EAX
// #define REG_ARG0	REG_EBX
// #define REG_ARG1	REG_ECX
// #define REG_ARG2	REG_EDX
// #define REG_ARG3	REG_ESI
// #define REG_ARG4	REG_EDI
// #define REG_ARG5	REG_EBP
// #elif defined(__x86_64__)
// #define REG_RESULT	REG_RAX
// #define REG_SYSCALL	REG_RAX
// #define REG_ARG0	REG_RDI
// #define REG_ARG1	REG_RSI
// #define REG_ARG2	REG_RDX
// #define REG_ARG3	REG_R10
// #define REG_ARG4	REG_R8
// #define REG_ARG5	REG_R9
// #endif

// #ifndef PR_SET_NO_NEW_PRIVS
// #define PR_SET_NO_NEW_PRIVS 38
// #endif

// #ifndef SYS_SECCOMP
// #define SYS_SECCOMP 1
// #endif

// #define MAX_LOG_LEN 4096
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <omp.h>

#define MAX 1000

int main(int argc, char **argv)
{
	// Pointer to the file to be
    // read from
    FILE* fptr1;
    char c;
  
    // Stores the bytes to read
    char str[MAX];
    int i = 0, j, from, to;
	from = 0;
	to = 1000;
  
    // If the file exists and has
    // read permission
    // fptr1 = fopen(argv[1], "r");
    fptr1 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/6156/output557030438000", "r");
  
    if (fptr1 == NULL) {
        return 1;
    }
  
    // Input from the user range of
    // bytes inclusive of from and to
    // printf("Read bytes from: ");
    // scanf("%d", &from);
    // printf("Read bytes upto: ");
    // scanf("%d", &to);
  
    // Loop to read required byte
    // of file
    for (i = 0, j = 0; i <= to
                       && c != EOF;
         i++) {
  
        // Skip the bytes not required
        if (i >= from) {
            str[j] = c;
            j++;
        }
  
        // Get the characters
        c = fgetc(fptr1);
    }
	

	// Print byte info
	// char *data;
	char *prtp = (char *)str[0];

	//print from str[0]
	printf("\nLOGGING: str[0] to 1000 offset\n");
	uint16_t counter = 0;
	// while (prtp != '\0') {
	while (counter < to) {
		if (counter >= to) {
			printf("\n Safety break, counter >= target");
			break;
		}
		printf("%02hhx ", *prtp);
		++counter;
		if (counter % 4 == 0)
			printf("\n");
		++prtp;
	}
  
    // Print the bytes as string
    // printf("%s", str);
  
    // Close the file
    fclose(fptr1);
  
    return 0;
}