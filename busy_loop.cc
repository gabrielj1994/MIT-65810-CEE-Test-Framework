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


int main(int argc, char **argv)
{
	cpu_set_t set;
	CPU_ZERO(&set);        // clear cpu mask
	CPU_SET(1, &set);      // set cpu 0
	sched_setaffinity(0, sizeof(cpu_set_t), &set);  // 0 is the calling process

	// Busy Loop
	time_t start, end;
    double runTime;
    int limit = 100000;
	int primes_arr[10000]; // expected 9592 with limit 100000
	while (true) {
		start = time(NULL);
		int num = 1,primes = 0;


	// #pragma omp parallel for schedule(dynamic) reduction(+ : primes)
		for (num = 1; num <= limit; num++) { 
			int i = 2; 
			while(i <= num) { 
				if(num % i == 0)
					break;
				i++; 
			}
			if(i == num) {
				primes_arr[primes] = num;
				primes++;
			}

			if(primes % 20000 == 0)
				sleep(1);
	//      printf("%d prime numbers calculated\n",primes);
		}

		end = time(NULL);
		runTime = end - start;
		printf("This machine calculated all %d prime numbers under %d in %g seconds\n",primes,limit,runTime);
		// ADD Registers
		register int reg1 = 2147483647;
		register bool reg2 = true;
	

		sleep(120);
	}
    

    return 0;
}