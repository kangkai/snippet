#include <stdio.h>

/* http://www.cse.yorku.ca/~oz/hash.html */
unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int main(int argc, char **argv)
{
	unsigned long i;

	if (argc != 2) {
		printf("Usage: %s string\n", argv[0]);
		return 0;
	}

	i = hash(argv[1]);
	printf("Hash of string: %lu, hash/313: %lu\n", i, i%313);
	return;
}
