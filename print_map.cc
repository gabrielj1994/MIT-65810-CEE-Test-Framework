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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
// #include <omp.h>

#define MAX 2000
#define SIZE 20
#define MAX_REGION_COUNT 50
#define MAX_WHITELIST_COUNT 50

// Hard-Coded Whitelists
// Hash-Table Implementation: https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm
// TODO: Consider a simple bit-map over hash table

struct DataItem {
   int data;   
   bool key;
};

struct DataItem** regionWhiteLists[MAX_REGION_COUNT];
// struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

int hashCode(int key) {
   return key % SIZE;
}

struct DataItem *hash_table_search(DataItem** hashArray, int key) {
   //get the hash 
   int hashIndex = hashCode(key);  
	
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex]; 
			
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

void hash_table_insert(DataItem** hashArray, int key, bool data) {

   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->data = data;  
   item->key = key;

   //get the hash 
   int hashIndex = hashCode(key);

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }
	
   hashArray[hashIndex] = item;
}

struct DataItem* hash_table_delete(DataItem** hashArray, struct DataItem* item) {
   int key = item->key;

   //get the hash 
   int hashIndex = hashCode(key);

   //move in array until an empty
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key) {
         struct DataItem* temp = hashArray[hashIndex]; 
			
         //assign a dummy item at deleted position
         hashArray[hashIndex] = dummyItem; 
         return temp;
      }
		
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }      
	
   return NULL;        
}

void inclusive_range_insert(DataItem** hashArray, int start_key, int end_key, bool data) {
	for (int i = start_key; i < end_key+1; i++) {
		hash_table_insert(hashArray, i, data);
	}
}

void setup_differences_whitelist() {
	dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
	dummyItem->data = -1;  
	dummyItem->key = false;
	for (int i = 0; i < MAX_REGION_COUNT; i++) {
		// NOTE: Maximum 50 regions. Each region can have a maximum 50 white list items
		regionWhiteLists[i] = (struct DataItem**) malloc(sizeof(struct DataItem*) * MAX_WHITELIST_COUNT);
		switch(i) {
		case 1:
		case 2:
		case 3:
		case 4:
			// No white list differences observed
			break; /* optional */
		case 5:
		  	// hash_table_insert(regionWhiteLists[i], 9, true);
		  	// hash_table_insert(regionWhiteLists[i], 10, true);
		  	// hash_table_insert(regionWhiteLists[i], 11, true);
		  	// hash_table_insert(regionWhiteLists[i], 12, true);
		  	// hash_table_insert(regionWhiteLists[i], 13, true);
			inclusive_range_insert(regionWhiteLists[i], 9, 13, true);
			break;
		default:
			// No white list differences observed elsewhere
			break;
		}
	}
}

int main(int argc, char **argv)
{
	printf("\nArg count: %d\n", argc);
	/*
	arg1: dump filepath1
	arg2: dump filepath2
	arg3: region index being compared (simple approach to handling whitelist) 
	*/
	if (argc < 4) {
		printf("\nInsufficient args. 3 required.");
		printf("\narg1: dump filepath1");
		printf("\narg2: dump filepath2");
		printf("\narg3: region index being compared (simple approach to handling whitelist)");
		exit(0);
	}

	// Pointer to the file to be
    // read from
    FILE* fptr1;
    FILE* fptr2;
    char c, d;
  
    // Stores the bytes to read
    char str1[MAX];
    char str2[MAX];
    int i = 0, j, from, to, region_index;
	from = 0;
	to = 1000;
	setup_differences_whitelist();


	printf("\nComparing memory content from:\n%s\n%s", argv[1], argv[2]);
	sscanf(argv[3], "%d", &region_index)

    // If the file exists and has
    // read permission
    // fptr1 = fopen(argv[1], "r");
    // fptr1 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8424/output55fca01b6000", "r");
    // fptr1 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8424/output55fca01b3000", "r");
    // fptr1 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8424/output55fca01b4000", "r");
    // fptr1 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8424/output55fca01b5000", "r");
    // fptr1 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8424/output55fca01b7000", "r");
    fptr1 = fopen(argv[1], "r");
// 	./dump_busy_loop/8424/output55fca01b3000  ./dump_busy_loop/8424/output55fca01b5000 
// ./dump_busy_loop/8424/output55fca01b4000  ./dump_busy_loop/8424/output55fca01b6000  ./dump_busy_loop/8424/output55fca01b7000

	/*
	location: 96
byte 1: 04
byte 2: 00

Byte difference
location: 100
byte 1: 04
byte 2: 00

Byte difference
location: 104
byte 1: 70
byte 2: 53
	*/
  
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
	c = fgetc(fptr1);
    for (i = 0, j = 0; i <= to
                       && c != EOF;
         i++) {
		
		str1[i] = c;
		// printf("str1 value loc=%d value=%c", i, c);

		// if (i == 96 || i == 100 || i == 104) {
		// 	printf("str1 value loc=%d value=%c", i, c);
		// }
  
        // Skip the bytes not required
        // if (i >= from) {
		// 	if (j == 96 || j == 97 || j == 108) {
		// 		printf("str1 value loc=%d value=%c", j, c);
		// 	}
        //     str1[j] = c;
        //     j++;
        // }
  
        // Get the characters
        c = fgetc(fptr1);
    }
	

	// Print byte info
	// char *data;
	// char *prtp = (char *)str1;

	// //print from str[0]
	// printf("\nLOGGING: str[0] to 1000 offset\n");
	// int counter = 0;
	// while (prtp != '\0') {
	// while (counter < to) {
	// 	if (counter >= to) {
	// 		printf("\n Safety break, counter >= target");
	// 		break;
	// 	}
	// 	printf("%02hhx ", *prtp);
	// 	++counter;
	// 	if (counter % 4 == 0)
	// 		printf("\n");
	// 	++prtp;
	// }
  
    // Print the bytes as string
    // printf("%s", str);
  
    // Close the file
    fclose(fptr1);
	// fptr2 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8572/output5629db5b2000", "r");
	// fptr2 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8572/output5629db5af000", "r");
	// fptr2 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8572/output5629db5b0000", "r");
	// fptr2 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8572/output5629db5b1000", "r");
	// fptr2 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8572/output5629db5b3000", "r");
	fptr2 = fopen(argv[2], "r");


	// fptr2 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8424/output55fca01b6000_2", "r");
	// fptr2 = fopen("/opt/GIT/MIT-65810-CEE-Test-Framework/dump_busy_loop/8424/output55fca01b6000", "r");
// 	./dump_busy_loop/8572/output5629db5af000  ./dump_busy_loop/8572/output5629db5b1000  ./dump_busy_loop/8572/output5629db5b3000
// ./dump_busy_loop/8572/output5629db5b0000  ./dump_busy_loop/8572/output5629db5b2000
  
    if (fptr2 == NULL) {
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
	d = fgetc(fptr2);
	int a = 0;
	int f = 0;
    for (a = 0, f = 0; a <= to
                       && d != EOF;
         a++) {

		str2[a] = d;
		// printf("str2 value loc=%d value=%c", a, d);

		// if (a == 96 || a == 100 || a == 104) {
		// 	printf("str1 value loc=%d value=%c", a, d);
		// }
  
        // Skip the bytes not required
        // if (i >= from) {
		// 	if (j == 96 || j == 97 || j == 108) {
		// 		printf("str2 value loc=%d value=%c", j, d);
		// 	}
        //     str2[j] = d;
        //     j++;
        // }
  
        // Get the characters
        d = fgetc(fptr2);
    }
	// Close the file
    fclose(fptr2);

	// Print byte info
	// char *data;
	// char *prtp1 = (char *)str1;
	// char *prtp2 = (char *)str2;

	// TODO: Add to white list 9-13 of 5th memory region (the rw region)

	//print from str[0]
	printf("\nLOGGING: str[0] to %d offset\n", i);
	int dif_counter = 0;
	int counter = 0;
	// while (prtp != '\0') {
	while (counter < i) {
		if (counter >= i) {
			printf("\n Safety break, counter >= target");
			break;
		}

		// if (*prtp1 != *prtp2) {
		if (str1[counter] != str2[counter]) {
			if (region_index < MAX_REGION_COUNT && hash_table_search(regionWhiteLists[region_index], counter) == NULL)
				continue;
			printf("\nByte difference");
			printf("\nlocation: %d", counter);
			printf("\nbyte 1: %02hhx", str1[counter]);
			printf("\nbyte 2: %02hhx\n", str2[counter]);
			++dif_counter;
		}
		// printf("%02hhx ", *prtp);
		++counter;
		// if (counter % 4 == 0)
		// 	printf("\n");
		// ++prtp1;
		// ++prtp2;
	}
	printf("\nTotal differences: %d\n", dif_counter);
    return 0;
}