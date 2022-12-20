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
#define MAX_REGION_COUNT 50
#define MAX_WHITELIST_COUNT 1000

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
   return key % MAX_WHITELIST_COUNT;
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
	printf("\nAdding whitelist [key:%d]", key);


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
		printf("\nAdding whitelist [region:%d]", i);

		// NOTE: Maximum 50 regions. Each region can have a maximum 50 white list items
		regionWhiteLists[i] = (struct DataItem**) malloc(sizeof(struct DataItem*) * MAX_WHITELIST_COUNT);
		switch(i) {
		case 3:
			inclusive_range_insert(regionWhiteLists[i], 9, 13, true);
			break;
		case 8:
			hash_table_insert(regionWhiteLists[i], 9, true);
			hash_table_insert(regionWhiteLists[i], 10, true);
			hash_table_insert(regionWhiteLists[i], 11, true);
			hash_table_insert(regionWhiteLists[i], 12, true);
			hash_table_insert(regionWhiteLists[i], 17, true);
			hash_table_insert(regionWhiteLists[i], 18, true);
			hash_table_insert(regionWhiteLists[i], 19, true);
			hash_table_insert(regionWhiteLists[i], 20, true);
			hash_table_insert(regionWhiteLists[i], 25, true);
			hash_table_insert(regionWhiteLists[i], 26, true);
			hash_table_insert(regionWhiteLists[i], 27, true);
			hash_table_insert(regionWhiteLists[i], 28, true);
			hash_table_insert(regionWhiteLists[i], 33, true);
			hash_table_insert(regionWhiteLists[i], 34, true);
			hash_table_insert(regionWhiteLists[i], 35, true);
			hash_table_insert(regionWhiteLists[i], 36, true);
			hash_table_insert(regionWhiteLists[i], 41, true);
			hash_table_insert(regionWhiteLists[i], 42, true);
			hash_table_insert(regionWhiteLists[i], 43, true);
			hash_table_insert(regionWhiteLists[i], 44, true);
			hash_table_insert(regionWhiteLists[i], 49, true);
			hash_table_insert(regionWhiteLists[i], 50, true);
			hash_table_insert(regionWhiteLists[i], 51, true);
			hash_table_insert(regionWhiteLists[i], 52, true);
			hash_table_insert(regionWhiteLists[i], 57, true);
			hash_table_insert(regionWhiteLists[i], 58, true);
			hash_table_insert(regionWhiteLists[i], 59, true);
			hash_table_insert(regionWhiteLists[i], 60, true);
			hash_table_insert(regionWhiteLists[i], 65, true);
			hash_table_insert(regionWhiteLists[i], 66, true);
			hash_table_insert(regionWhiteLists[i], 67, true);
			hash_table_insert(regionWhiteLists[i], 68, true);
			hash_table_insert(regionWhiteLists[i], 73, true);
			hash_table_insert(regionWhiteLists[i], 74, true);
			hash_table_insert(regionWhiteLists[i], 75, true);
			hash_table_insert(regionWhiteLists[i], 76, true);
			hash_table_insert(regionWhiteLists[i], 81, true);
			hash_table_insert(regionWhiteLists[i], 82, true);
			hash_table_insert(regionWhiteLists[i], 83, true);
			hash_table_insert(regionWhiteLists[i], 84, true);
			hash_table_insert(regionWhiteLists[i], 89, true);
			hash_table_insert(regionWhiteLists[i], 90, true);
			hash_table_insert(regionWhiteLists[i], 91, true);
			hash_table_insert(regionWhiteLists[i], 92, true);
			hash_table_insert(regionWhiteLists[i], 97, true);
			hash_table_insert(regionWhiteLists[i], 98, true);
			hash_table_insert(regionWhiteLists[i], 99, true);
			hash_table_insert(regionWhiteLists[i], 100, true);
			hash_table_insert(regionWhiteLists[i], 105, true);
			hash_table_insert(regionWhiteLists[i], 106, true);
			hash_table_insert(regionWhiteLists[i], 107, true);
			hash_table_insert(regionWhiteLists[i], 108, true);
			hash_table_insert(regionWhiteLists[i], 113, true);
			hash_table_insert(regionWhiteLists[i], 114, true);
			hash_table_insert(regionWhiteLists[i], 115, true);
			hash_table_insert(regionWhiteLists[i], 116, true);
			hash_table_insert(regionWhiteLists[i], 121, true);
			hash_table_insert(regionWhiteLists[i], 122, true);
			hash_table_insert(regionWhiteLists[i], 123, true);
			hash_table_insert(regionWhiteLists[i], 124, true);
			hash_table_insert(regionWhiteLists[i], 129, true);
			hash_table_insert(regionWhiteLists[i], 130, true);
			hash_table_insert(regionWhiteLists[i], 131, true);
			hash_table_insert(regionWhiteLists[i], 132, true);
			hash_table_insert(regionWhiteLists[i], 137, true);
			hash_table_insert(regionWhiteLists[i], 138, true);
			hash_table_insert(regionWhiteLists[i], 139, true);
			hash_table_insert(regionWhiteLists[i], 140, true);
			hash_table_insert(regionWhiteLists[i], 145, true);
			hash_table_insert(regionWhiteLists[i], 146, true);
			hash_table_insert(regionWhiteLists[i], 147, true);
			hash_table_insert(regionWhiteLists[i], 148, true);
			hash_table_insert(regionWhiteLists[i], 153, true);
			hash_table_insert(regionWhiteLists[i], 154, true);
			hash_table_insert(regionWhiteLists[i], 155, true);
			hash_table_insert(regionWhiteLists[i], 156, true);
			hash_table_insert(regionWhiteLists[i], 161, true);
			hash_table_insert(regionWhiteLists[i], 162, true);
			hash_table_insert(regionWhiteLists[i], 163, true);
			hash_table_insert(regionWhiteLists[i], 164, true);
			hash_table_insert(regionWhiteLists[i], 169, true);
			hash_table_insert(regionWhiteLists[i], 170, true);
			hash_table_insert(regionWhiteLists[i], 171, true);
			hash_table_insert(regionWhiteLists[i], 172, true);
			hash_table_insert(regionWhiteLists[i], 177, true);
			hash_table_insert(regionWhiteLists[i], 178, true);
			hash_table_insert(regionWhiteLists[i], 179, true);
			hash_table_insert(regionWhiteLists[i], 180, true);
			hash_table_insert(regionWhiteLists[i], 185, true);
			hash_table_insert(regionWhiteLists[i], 186, true);
			hash_table_insert(regionWhiteLists[i], 187, true);
			hash_table_insert(regionWhiteLists[i], 188, true);
			hash_table_insert(regionWhiteLists[i], 193, true);
			hash_table_insert(regionWhiteLists[i], 194, true);
			hash_table_insert(regionWhiteLists[i], 195, true);
			hash_table_insert(regionWhiteLists[i], 196, true);
			hash_table_insert(regionWhiteLists[i], 201, true);
			hash_table_insert(regionWhiteLists[i], 202, true);
			hash_table_insert(regionWhiteLists[i], 203, true);
			hash_table_insert(regionWhiteLists[i], 204, true);
			hash_table_insert(regionWhiteLists[i], 209, true);
			hash_table_insert(regionWhiteLists[i], 210, true);
			hash_table_insert(regionWhiteLists[i], 211, true);
			hash_table_insert(regionWhiteLists[i], 212, true);
			hash_table_insert(regionWhiteLists[i], 217, true);
			hash_table_insert(regionWhiteLists[i], 218, true);
			hash_table_insert(regionWhiteLists[i], 219, true);
			hash_table_insert(regionWhiteLists[i], 220, true);
			hash_table_insert(regionWhiteLists[i], 225, true);
			hash_table_insert(regionWhiteLists[i], 226, true);
			hash_table_insert(regionWhiteLists[i], 227, true);
			hash_table_insert(regionWhiteLists[i], 228, true);
			hash_table_insert(regionWhiteLists[i], 233, true);
			hash_table_insert(regionWhiteLists[i], 234, true);
			hash_table_insert(regionWhiteLists[i], 235, true);
			hash_table_insert(regionWhiteLists[i], 236, true);
			hash_table_insert(regionWhiteLists[i], 241, true);
			hash_table_insert(regionWhiteLists[i], 242, true);
			hash_table_insert(regionWhiteLists[i], 243, true);
			hash_table_insert(regionWhiteLists[i], 244, true);
			hash_table_insert(regionWhiteLists[i], 249, true);
			hash_table_insert(regionWhiteLists[i], 250, true);
			hash_table_insert(regionWhiteLists[i], 251, true);
			hash_table_insert(regionWhiteLists[i], 252, true);
			hash_table_insert(regionWhiteLists[i], 257, true);
			hash_table_insert(regionWhiteLists[i], 258, true);
			hash_table_insert(regionWhiteLists[i], 259, true);
			hash_table_insert(regionWhiteLists[i], 260, true);
			hash_table_insert(regionWhiteLists[i], 265, true);
			hash_table_insert(regionWhiteLists[i], 266, true);
			hash_table_insert(regionWhiteLists[i], 267, true);
			hash_table_insert(regionWhiteLists[i], 268, true);
			hash_table_insert(regionWhiteLists[i], 273, true);
			hash_table_insert(regionWhiteLists[i], 274, true);
			hash_table_insert(regionWhiteLists[i], 275, true);
			hash_table_insert(regionWhiteLists[i], 276, true);
			hash_table_insert(regionWhiteLists[i], 281, true);
			hash_table_insert(regionWhiteLists[i], 282, true);
			hash_table_insert(regionWhiteLists[i], 283, true);
			hash_table_insert(regionWhiteLists[i], 284, true);
			hash_table_insert(regionWhiteLists[i], 289, true);
			hash_table_insert(regionWhiteLists[i], 290, true);
			hash_table_insert(regionWhiteLists[i], 291, true);
			hash_table_insert(regionWhiteLists[i], 292, true);
			hash_table_insert(regionWhiteLists[i], 297, true);
			hash_table_insert(regionWhiteLists[i], 298, true);
			hash_table_insert(regionWhiteLists[i], 299, true);
			hash_table_insert(regionWhiteLists[i], 300, true);
			hash_table_insert(regionWhiteLists[i], 305, true);
			hash_table_insert(regionWhiteLists[i], 306, true);
			hash_table_insert(regionWhiteLists[i], 307, true);
			hash_table_insert(regionWhiteLists[i], 308, true);
			hash_table_insert(regionWhiteLists[i], 313, true);
			hash_table_insert(regionWhiteLists[i], 314, true);
			hash_table_insert(regionWhiteLists[i], 315, true);
			hash_table_insert(regionWhiteLists[i], 316, true);
			hash_table_insert(regionWhiteLists[i], 321, true);
			hash_table_insert(regionWhiteLists[i], 322, true);
			hash_table_insert(regionWhiteLists[i], 323, true);
			hash_table_insert(regionWhiteLists[i], 324, true);
			hash_table_insert(regionWhiteLists[i], 329, true);
			hash_table_insert(regionWhiteLists[i], 330, true);
			hash_table_insert(regionWhiteLists[i], 331, true);
			hash_table_insert(regionWhiteLists[i], 332, true);
			hash_table_insert(regionWhiteLists[i], 337, true);
			hash_table_insert(regionWhiteLists[i], 338, true);
			hash_table_insert(regionWhiteLists[i], 339, true);
			hash_table_insert(regionWhiteLists[i], 340, true);
			hash_table_insert(regionWhiteLists[i], 345, true);
			hash_table_insert(regionWhiteLists[i], 346, true);
			hash_table_insert(regionWhiteLists[i], 347, true);
			hash_table_insert(regionWhiteLists[i], 348, true);
			hash_table_insert(regionWhiteLists[i], 353, true);
			hash_table_insert(regionWhiteLists[i], 354, true);
			hash_table_insert(regionWhiteLists[i], 355, true);
			hash_table_insert(regionWhiteLists[i], 356, true);
			hash_table_insert(regionWhiteLists[i], 361, true);
			hash_table_insert(regionWhiteLists[i], 362, true);
			hash_table_insert(regionWhiteLists[i], 363, true);
			hash_table_insert(regionWhiteLists[i], 364, true);
			hash_table_insert(regionWhiteLists[i], 369, true);
			hash_table_insert(regionWhiteLists[i], 370, true);
			hash_table_insert(regionWhiteLists[i], 371, true);
			hash_table_insert(regionWhiteLists[i], 372, true);
			hash_table_insert(regionWhiteLists[i], 377, true);
			hash_table_insert(regionWhiteLists[i], 378, true);
			hash_table_insert(regionWhiteLists[i], 379, true);
			hash_table_insert(regionWhiteLists[i], 380, true);
			hash_table_insert(regionWhiteLists[i], 385, true);
			hash_table_insert(regionWhiteLists[i], 386, true);
			hash_table_insert(regionWhiteLists[i], 387, true);
			hash_table_insert(regionWhiteLists[i], 388, true);
			hash_table_insert(regionWhiteLists[i], 393, true);
			hash_table_insert(regionWhiteLists[i], 394, true);
			hash_table_insert(regionWhiteLists[i], 395, true);
			hash_table_insert(regionWhiteLists[i], 396, true);
			break;
		case 13:
			inclusive_range_insert(regionWhiteLists[i], 25, 28, true);
			inclusive_range_insert(regionWhiteLists[i], 33, 36, true);
			inclusive_range_insert(regionWhiteLists[i], 41, 44, true);
			inclusive_range_insert(regionWhiteLists[i], 49, 52, true);
			inclusive_range_insert(regionWhiteLists[i], 57, 60, true);
			inclusive_range_insert(regionWhiteLists[i], 65, 68, true);
			inclusive_range_insert(regionWhiteLists[i], 73, 76, true);
			inclusive_range_insert(regionWhiteLists[i], 97, 100, true);
			inclusive_range_insert(regionWhiteLists[i], 113, 116, true);
			break;
		case 14:
			hash_table_insert(regionWhiteLists[i], 1, true);
			hash_table_insert(regionWhiteLists[i], 2, true);
			hash_table_insert(regionWhiteLists[i], 3, true);
			hash_table_insert(regionWhiteLists[i], 4, true);
			hash_table_insert(regionWhiteLists[i], 32, true);
			hash_table_insert(regionWhiteLists[i], 33, true);
			hash_table_insert(regionWhiteLists[i], 34, true);
			hash_table_insert(regionWhiteLists[i], 81, true);
			hash_table_insert(regionWhiteLists[i], 82, true);
			hash_table_insert(regionWhiteLists[i], 83, true);
			hash_table_insert(regionWhiteLists[i], 84, true);
			hash_table_insert(regionWhiteLists[i], 85, true);
			hash_table_insert(regionWhiteLists[i], 89, true);
			hash_table_insert(regionWhiteLists[i], 90, true);
			hash_table_insert(regionWhiteLists[i], 91, true);
			hash_table_insert(regionWhiteLists[i], 92, true);
			hash_table_insert(regionWhiteLists[i], 209, true);
			hash_table_insert(regionWhiteLists[i], 210, true);
			hash_table_insert(regionWhiteLists[i], 211, true);
			hash_table_insert(regionWhiteLists[i], 212, true);
			hash_table_insert(regionWhiteLists[i], 233, true);
			hash_table_insert(regionWhiteLists[i], 234, true);
			hash_table_insert(regionWhiteLists[i], 235, true);
			hash_table_insert(regionWhiteLists[i], 236, true);
			hash_table_insert(regionWhiteLists[i], 241, true);
			hash_table_insert(regionWhiteLists[i], 242, true);
			hash_table_insert(regionWhiteLists[i], 243, true);
			hash_table_insert(regionWhiteLists[i], 244, true);
			hash_table_insert(regionWhiteLists[i], 249, true);
			hash_table_insert(regionWhiteLists[i], 250, true);
			hash_table_insert(regionWhiteLists[i], 251, true);
			hash_table_insert(regionWhiteLists[i], 252, true);
			hash_table_insert(regionWhiteLists[i], 273, true);
			hash_table_insert(regionWhiteLists[i], 274, true);
			hash_table_insert(regionWhiteLists[i], 275, true);
			hash_table_insert(regionWhiteLists[i], 276, true);
			hash_table_insert(regionWhiteLists[i], 277, true);
			hash_table_insert(regionWhiteLists[i], 312, true);
			hash_table_insert(regionWhiteLists[i], 313, true);
			hash_table_insert(regionWhiteLists[i], 314, true);
			hash_table_insert(regionWhiteLists[i], 315, true);
			hash_table_insert(regionWhiteLists[i], 361, true);
			hash_table_insert(regionWhiteLists[i], 362, true);
			hash_table_insert(regionWhiteLists[i], 363, true);
			hash_table_insert(regionWhiteLists[i], 364, true);
			hash_table_insert(regionWhiteLists[i], 369, true);
			hash_table_insert(regionWhiteLists[i], 370, true);
			hash_table_insert(regionWhiteLists[i], 371, true);
			hash_table_insert(regionWhiteLists[i], 372, true);
			hash_table_insert(regionWhiteLists[i], 385, true);
			hash_table_insert(regionWhiteLists[i], 386, true);
			hash_table_insert(regionWhiteLists[i], 387, true);
			hash_table_insert(regionWhiteLists[i], 388, true);
			hash_table_insert(regionWhiteLists[i], 401, true);
			hash_table_insert(regionWhiteLists[i], 402, true);
			hash_table_insert(regionWhiteLists[i], 403, true);
			hash_table_insert(regionWhiteLists[i], 404, true);
			hash_table_insert(regionWhiteLists[i], 405, true);
			hash_table_insert(regionWhiteLists[i], 409, true);
			hash_table_insert(regionWhiteLists[i], 410, true);
			hash_table_insert(regionWhiteLists[i], 411, true);
			hash_table_insert(regionWhiteLists[i], 412, true);
			hash_table_insert(regionWhiteLists[i], 417, true);
			hash_table_insert(regionWhiteLists[i], 418, true);
			hash_table_insert(regionWhiteLists[i], 419, true);
			hash_table_insert(regionWhiteLists[i], 420, true);
			hash_table_insert(regionWhiteLists[i], 421, true);
			hash_table_insert(regionWhiteLists[i], 425, true);
			hash_table_insert(regionWhiteLists[i], 426, true);
			hash_table_insert(regionWhiteLists[i], 427, true);
			hash_table_insert(regionWhiteLists[i], 428, true);
			hash_table_insert(regionWhiteLists[i], 441, true);
			hash_table_insert(regionWhiteLists[i], 442, true);
			hash_table_insert(regionWhiteLists[i], 443, true);
			hash_table_insert(regionWhiteLists[i], 444, true);
			hash_table_insert(regionWhiteLists[i], 457, true);
			hash_table_insert(regionWhiteLists[i], 458, true);
			hash_table_insert(regionWhiteLists[i], 459, true);
			hash_table_insert(regionWhiteLists[i], 460, true);
			hash_table_insert(regionWhiteLists[i], 473, true);
			hash_table_insert(regionWhiteLists[i], 474, true);
			hash_table_insert(regionWhiteLists[i], 475, true);
			hash_table_insert(regionWhiteLists[i], 476, true);
			hash_table_insert(regionWhiteLists[i], 477, true);
			hash_table_insert(regionWhiteLists[i], 481, true);
			hash_table_insert(regionWhiteLists[i], 482, true);
			hash_table_insert(regionWhiteLists[i], 483, true);
			hash_table_insert(regionWhiteLists[i], 484, true);
			hash_table_insert(regionWhiteLists[i], 485, true);
			hash_table_insert(regionWhiteLists[i], 489, true);
			hash_table_insert(regionWhiteLists[i], 490, true);
			hash_table_insert(regionWhiteLists[i], 491, true);
			hash_table_insert(regionWhiteLists[i], 492, true);
			hash_table_insert(regionWhiteLists[i], 493, true);
			hash_table_insert(regionWhiteLists[i], 505, true);
			hash_table_insert(regionWhiteLists[i], 506, true);
			hash_table_insert(regionWhiteLists[i], 507, true);
			hash_table_insert(regionWhiteLists[i], 508, true);
			hash_table_insert(regionWhiteLists[i], 509, true);
			hash_table_insert(regionWhiteLists[i], 513, true);
			hash_table_insert(regionWhiteLists[i], 514, true);
			hash_table_insert(regionWhiteLists[i], 515, true);
			hash_table_insert(regionWhiteLists[i], 516, true);
			hash_table_insert(regionWhiteLists[i], 517, true);
			hash_table_insert(regionWhiteLists[i], 521, true);
			hash_table_insert(regionWhiteLists[i], 522, true);
			hash_table_insert(regionWhiteLists[i], 523, true);
			hash_table_insert(regionWhiteLists[i], 524, true);
			hash_table_insert(regionWhiteLists[i], 525, true);
			hash_table_insert(regionWhiteLists[i], 529, true);
			hash_table_insert(regionWhiteLists[i], 530, true);
			hash_table_insert(regionWhiteLists[i], 531, true);
			hash_table_insert(regionWhiteLists[i], 532, true);
			hash_table_insert(regionWhiteLists[i], 533, true);
			hash_table_insert(regionWhiteLists[i], 537, true);
			hash_table_insert(regionWhiteLists[i], 538, true);
			hash_table_insert(regionWhiteLists[i], 539, true);
			hash_table_insert(regionWhiteLists[i], 540, true);
			hash_table_insert(regionWhiteLists[i], 541, true);
			hash_table_insert(regionWhiteLists[i], 545, true);
			hash_table_insert(regionWhiteLists[i], 546, true);
			hash_table_insert(regionWhiteLists[i], 547, true);
			hash_table_insert(regionWhiteLists[i], 548, true);
			hash_table_insert(regionWhiteLists[i], 549, true);
			hash_table_insert(regionWhiteLists[i], 553, true);
			hash_table_insert(regionWhiteLists[i], 554, true);
			hash_table_insert(regionWhiteLists[i], 555, true);
			hash_table_insert(regionWhiteLists[i], 556, true);
			hash_table_insert(regionWhiteLists[i], 557, true);
			hash_table_insert(regionWhiteLists[i], 561, true);
			hash_table_insert(regionWhiteLists[i], 562, true);
			hash_table_insert(regionWhiteLists[i], 563, true);
			hash_table_insert(regionWhiteLists[i], 564, true);
			hash_table_insert(regionWhiteLists[i], 565, true);
			hash_table_insert(regionWhiteLists[i], 569, true);
			hash_table_insert(regionWhiteLists[i], 570, true);
			hash_table_insert(regionWhiteLists[i], 571, true);
			hash_table_insert(regionWhiteLists[i], 572, true);
			hash_table_insert(regionWhiteLists[i], 573, true);
			hash_table_insert(regionWhiteLists[i], 625, true);
			hash_table_insert(regionWhiteLists[i], 626, true);
			hash_table_insert(regionWhiteLists[i], 627, true);
			hash_table_insert(regionWhiteLists[i], 628, true);
			hash_table_insert(regionWhiteLists[i], 629, true);
			hash_table_insert(regionWhiteLists[i], 633, true);
			hash_table_insert(regionWhiteLists[i], 634, true);
			hash_table_insert(regionWhiteLists[i], 635, true);
			hash_table_insert(regionWhiteLists[i], 636, true);
			hash_table_insert(regionWhiteLists[i], 637, true);
			hash_table_insert(regionWhiteLists[i], 649, true);
			hash_table_insert(regionWhiteLists[i], 650, true);
			hash_table_insert(regionWhiteLists[i], 651, true);
			hash_table_insert(regionWhiteLists[i], 652, true);
			hash_table_insert(regionWhiteLists[i], 653, true);
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
	sscanf(argv[3], "%d", &region_index);

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
			if (region_index < MAX_REGION_COUNT && hash_table_search(regionWhiteLists[region_index], counter) != NULL) {
				++counter;
				continue;
			}
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