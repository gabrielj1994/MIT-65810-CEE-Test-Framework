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
#include<math.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <omp.h>

int x, y, n, t, i, flag;
long int e[50], d[50], temp[50], j, m[50], en[50];
char msg[100];
char msg_original[100];
char msg_final[100];
int prime(long int);
void encryption_key();
long int cd(long int);
void encrypt();
void decrypt();

int prime(long int pr)
{
  int i;
  j = sqrt(pr);
  for(i = 2; i <= j; i++)
  {
   if(pr % i == 0)
     return 0;
  }
  return 1;
 }

//function to generate encryption key
void encryption_key()
{
  int k;
  k = 0;
  for(i = 2; i < t; i++)
  {
    if(t % i == 0)
     continue;
    flag = prime(i);
    if(flag == 1 && i != x && i != y)
    {
     e[k] = i;
     flag = cd(e[k]);
    if(flag > 0)
    {
     d[k] = flag;
     k++;
    }
   if(k == 99)
    break;
   }
 }
}
long int cd(long int a)
{
  long int k = 1;
  while(1)
  {
    k = k + t;
    if(k % a == 0)
     return(k / a);
  }
}

//function to encrypt the message
void encrypt()
{
  long int pt, ct, key = e[0], k, len;
  i = 0;
  len = strlen(msg);
  while(i != len)
  {
    pt = m[i];
    pt = pt - 96;
    k = 1;
    for(j = 0; j < key; j++)
    {
     k = k * pt;
     k = k % n;
    }
   temp[i] = k;
   ct = k + 96;
   en[i] = ct;
   i++;
  }
  en[i] = -1;
  printf("\n\nTHE ENCRYPTED MESSAGE IS\n");
  for(i = 0; en[i] != -1; i++)
    printf("%ld", en[i]);
}

//function to decrypt the message
void decrypt()
{
  long int pt, ct, key = d[0], k;
  i = 0;
  while(en[i] != -1)
  {
    ct = temp[i];
    k = 1;
    for(j = 0; j < key; j++)
    {
      k = k * ct;
      k = k % n;
    }
   pt = k + 96;
   m[i] = pt;
   i++;
  }
  m[i] = -1;
  printf("\n\nTHE DECRYPTED MESSAGE IS\n");
  for(i = 0; m[i] != -1; i++)
   printf("%ld", m[i]);
  printf("\n");
}

int main(int argc, char **argv)
{
	printf("\nArg count: %d\n", argc);
	/*
	arg1: dump filepath1
	arg2: dump filepath2
	arg3: region index being compared (simple approach to handling whitelist) 
	*/
	if (argc < 3) {
		printf("\nInsufficient args. 2 required.");
		printf("\narg1: target core");
		printf("\narg2: divergent execution flag for odd cores");
		exit(0);
	}

	int target_core;
	sscanf(argv[1], "%d", &target_core);
	bool divergent_flag = (bool)(argv[2] - '0' > 0) && (target_core % 2) == 1;

	cpu_set_t set;
	CPU_ZERO(&set);        // clear cpu mask
	CPU_SET(target_core, &set);      // set cpu 1
	sched_setaffinity(0, sizeof(cpu_set_t), &set);  // 0 is the calling process

	// printf("\nENTER FIRST PRIME NUMBER\n");
	// scanf("%d", &x);
	x = divergent_flag ? 73 : 113;
	flag = prime(x);
	if(flag == 0)
	{
		printf("\nINVALID PRIME\n");
		exit(0);
	}
	// printf("\nENTER SECOND PRIME NUMBER\n");
	// scanf("%d", &y);
	y = divergent_flag ? 271 : 313;
	flag = prime(y);
	if(flag == 0 || x == y)
	{
		printf("\nINVALID PRIME\n");
		exit(0);
	}
	// printf("\nENTER MESSAGE OR STRING TO ENCRYPT\n");

	// scanf("%s",msg);
	while (true) {
		strcpy(msg, "This is a baseline string");
		strcpy(msg_original, msg);
		for(i = 0; msg[i] != NULL; i++)
			m[i] = msg[i];
		n = x * y;
		t = (x-1) * (y-1);
		encryption_key();
		printf("\nPOSSIBLE VALUES OF e AND d ARE\n");
		for(i = 0; i < j-1; i++)
			printf("\n%ld\t%ld", e[i], d[i]);
		encrypt();
		decrypt();

		strcpy(msg_final, (char*)m);
		for(i = 0; msg_final[i] != -1; i++) {
			if (msg_final[i] != msg_original[i]) {
				printf("Encryption process failure.");
				printf("Character index: %d", i);
				printf("Original character: %c", msg_original[i]);
				printf("Translated character: %c", msg_final[i]);
			}
		}

		sleep(300);
	}
    return 0;
}