#ifndef lint
static const char rcsid[] =
	"$Id: 1.c,v 1.3 2005/02/06 17:05:40 andrew Exp $";
#endif

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"

#define MAXLENGTH	2048

int main(int argc, char *argv[]) {
	char *rs,
		 string[MAXLENGTH];
	int num,
		*nump;
	hash h;
	struct hash_iterator it;

	if (! hash_initialise(&h, 997U, hash_hash_int, hash_compare_int, hash_copy_int, free, free)) {
		err(1, "initialise");
	}

	printf("Enter a positive number followed by a string to add to the hash.\n"
		   "Enter a positive number alone to retrieve.\n"
		   "Enter a negative number to remove.\n"
		   "Enter 0 to list all.\n\n");
	
	do {
		printf("> ");
		if (fgets(string, MAXLENGTH, stdin) == NULL) {
			break;
		}
		switch (sscanf(string, "%d %s", &num, string)) {
			case 0:
				/* error */
				printf("burp\n");
				return 0;
				break;
			case 1:
				/* retrieve or remove or list all */
				if (num < 0) {
					/* remove */
					num = -num;
					if (! hash_delete(&h, &num)) {
						printf("no entry for %d\n", num);
					} else {
						printf("entry for %d removed\n", num);
					}
				} else if (num > 0) {
					/* retrieve */
					if (! hash_retrieve(&h, &num, (void **)&rs)) {
						printf("no string stored for %d\n", num);
					} else {
						printf("string for %d is %s\n", num, rs);
					}
				} else {
					/* list all */
					if (! hash_iterator_initialise(&h, &it)) {
						err(1, "hash_iterator_initialise");
					}
					while (hash_fetch_next(&h, &it, (void **)&nump, (void **)&rs)) {
						printf("%d: %s\n", *nump, rs);
					}
					if (! hash_iterator_deinitialise(&h, &it)) {
						err(1, "hash_iterator_deinitialise");
					}
				}
				break;
			case 2:
				/* store */
				if ((rs = strdup(string)) == NULL) {
					err(1, "strdup");
				}
				if (! hash_insert(&h, &num, rs)) {
					err(1, "hash_insert");
				} else {
					printf("stored %s for %d\n", rs, num);
				}
				break;
		}
	} while (1);

	hash_deinitialise(&h);

	//sleep(10);

	if (feof(stdin)) {
		printf("\n");
		return 0;
	} else {
		printf("burp");
		return 1;
	}
}
