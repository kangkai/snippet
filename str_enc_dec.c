/* simple data encrypt/decrypt.
 *
 * (c) 2016 Xiaomi
 * Author: Yin Kangkai <yinkangkai@xiaomi.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <unistd.h>
#include <getopt.h>

/* key can be any chars, and any size array */
//const char key[] = {'t', 'e', 's', 't', 'k', 'e', 'y', '\0'};
//const char key[] = {'5', '/', 'd', '$', 'V', 'd', 'm', 'W', '6', ',', '~', '0', 'u', '>', '#', 'k', '\0'};
const char *key="89JFSjo8HUbhou5776NJOMp9i90ghg7Y78G78t68899y79HY7g7y87y9ED45Ew30O0jkkl";

static void str2hex(char *ds, unsigned char *bs, unsigned int n)
{
	int i;
	for (i = 0; i < n; i++)
		sprintf(ds + 2 * i, "%02x", bs[i]);

}

static void hex2str(char *buf, int buflen, const char *hex, int hexlen)
{
	char substr[3];
	int i;

	if (buflen < hexlen / 2) {
		printf("buflen(%d) might not be enough for the hexlen(%d) convertion.\n", buflen, hexlen);
		return;
	}

	memset(buf, 0, buflen);

	for (i = 0; i < buflen &&  i * 2 < hexlen; i++) {
		memcpy(substr, hex + i * 2, 2);
		substr[2] = '\0';
		buf[i] = (char)strtoul(substr, NULL, 16);
	}

	return;
}

static void xor_encrypt(const char *key, char *string, int n)
{
	int i;
	int len = strlen(key);
	for (i = 0 ; i < n ; i++) {
		string[i] = string[i] ^ key[i % len];
	}
}

/**
 * key: key used to encrypt data;
 * in: input data;
 * in_len: input data length;
 * out: output data;
 * out_len: output data length, initialized as out buffer length, and will
 *	be changed to reflect actual output data length;
 *
 * Note: out_len must be long enough: at least (2*in_len + 1)
 */
static int data_encrypt(const char *key, const char *in, int in_len,
			char *out, int *out_len)
{
	int i;
	char *tmp;

	if (*out_len < 2 * in_len + 1)
		return -1;
	if (key == NULL || in == NULL || out == NULL || in_len == 0)
		return -1;

	tmp = malloc(in_len);
	if (tmp == NULL)
		return -1;
	memcpy(tmp, in, in_len);
	xor_encrypt(key, tmp, in_len);
	for (i = 0; i < in_len; i++)
		tmp[i] -= 15;
	str2hex(out, tmp, in_len);
	*out_len = 2 * in_len;

	return 0;
}

/**
 * key: key used to decrypt data;
 * in: input data;
 * in_len: input data length;
 * out: output data;
 * out_len: output data length, initialized as out buffer length, and will
 *	be changed to reflect actual output data length;
 *
 * Note: out_len must be long enough: at least in_len / 2 
 */
static int data_decrypt(const char *key, const char *in, int in_len,
			char *out, int *out_len)
{
	int i;

	if (*out_len < in_len / 2)
		return -1;
	if (in_len % 2 != 0)
		return -1;
	if (key == NULL || in == NULL || out == NULL || in_len == 0)
		return -1;

	hex2str(out, *out_len, in, in_len);
	*out_len = in_len / 2;
	for (i = 0; i < *out_len; i++)
		out[i] += 15;
	xor_encrypt(key, out, *out_len);

	return 0;
}

static struct option options[] = {
	{"help",	no_argument,		NULL, 'h'},
	{"encrypt",	required_argument,      NULL, 'e'},
	{"decrypt",	required_argument,      NULL, 'd'},
	{"both",	required_argument,      NULL, 'b'},
	{NULL,		0,			0,	0}
};

int main(int argc, char **argv) {
	bool encrypt_flag, decrypt_flag;
	char data[PATH_MAX];
	char *enstr, *destr;
	char *out;
	int in_len, out_len;
	int i, n, ret;

	encrypt_flag = decrypt_flag = false;
	n = 0;
	while (n >= 0) {
		n = getopt_long(argc, argv, "e:d:b:h", options, NULL);
		if (n < 0)
			continue;
		switch (n) {
		case 'e':
			encrypt_flag = true;
			enstr = optarg;
			break;
		case 'd':
			decrypt_flag = true;
			destr = optarg;
			break;
		case 'b':
			encrypt_flag = true;
			decrypt_flag = true;
			enstr = optarg;
			break;
		case 'h':
		default:
			printf("Usage: %s\n"
			       "\t[-e --encrypt] string\n"
			       "\t[-d --decrypt] string\n"
			       "\t[-b --both] string\n"
			       , basename(argv[0]));
			exit(1);
		}
	}

	if (encrypt_flag) {
		if (strlen(enstr) >= sizeof(data) / 2) {
			printf("data too long: %s.\n", enstr);
			return -1;
		}
		in_len = strlen(enstr);
		printf("raw string: %s, len: %d\n", enstr, in_len);

		out_len = PATH_MAX;
		memset(data, 0, sizeof(data));
		ret = data_encrypt(key, enstr, in_len, data, &out_len);
		if (ret < 0) {
			printf("encrypt error.\n");
			return -1;
		}

		printf("encrypted string: %s, len: %d\n", data, out_len);
	}

	if (decrypt_flag) {
		if (destr != NULL) {
			in_len = strlen(destr);
		} else {
			in_len = out_len;
			destr = data;
		}

		out_len = in_len / 2 + 1;
		out = malloc(out_len);
		if (out == NULL) {
			perror("malloc");
			return -1;
		}
		memset(out, 0, out_len);
		ret = data_decrypt(key, destr, in_len, out, &out_len);
		if (ret < 0) {
			printf("decrypt error.\n");
			return -1;
		}

		printf("decrypted string: %s, len: %d\n", out, out_len);
		free(out);
	}

	return 0;
}
