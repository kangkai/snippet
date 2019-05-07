#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>


const uint8_t *rootkey = "65662c2d2c2f7509727624"; /* RUNNINGBEEF */
const char key[] = {'7', '3', 'b', 'c', 'e', 'a', '2', 'K', '\0'};

static void xor_encrypt(const char *key, char *string, int n)
{
	int i;
	int len = strlen(key);
	for (i = 0 ; i < n ; i++) {
		string[i] = string[i] ^ key[i % len];
		printf("%02x, i: %d, n: %d, len: %d, i%%len: %d\n",
		       string[i], i, n, len, (i % len));
	}
}

static void hex2str(char *buf, int buflen, const char *hex, int hexlen)
{
	char substr[3];
	int i;

	if (buflen < hexlen / 2) {
		fprintf(stderr, "buflen(%d) might not be enough for the hexlen(%d) convertion.\n", buflen, hexlen);
	}

	memset(buf, 0, buflen);

	for (i = 0; i < buflen &&  i * 2 < hexlen; i++) {
		memcpy(substr, hex + i * 2, 2);
		substr[2] = '\0';
		buf[i] = (char)strtoul(substr, NULL, 16);
	}

	return;
}

int main(int argc, char **argv)
{
	char *rootkey2;
	int i;


	/* rootkey */
	int keylen = strlen(rootkey);
	rootkey2 = malloc(keylen);
	memset(rootkey2, 0, keylen);
	hex2str(rootkey2, keylen, rootkey, keylen);

	keylen = keylen / 2;
	printf("rootkey: %s, rootkey2: %s, keylen: %d\n", rootkey, rootkey2, keylen);
	for (i = 0; i < keylen; i++)
		printf("%02x", rootkey2[i]);
	printf("\n");
/*
	for (i = 0; i < keylen; i++)
		rootkey2[i] += 15;
*/
	xor_encrypt(key, rootkey2, keylen);
	for (i = 0; i < keylen; i++)
		printf("%02x", rootkey2[i]);
	printf("\n");

	fprintf(stderr, "rootkey debug: %s\n", rootkey2);

	return 0;
}
