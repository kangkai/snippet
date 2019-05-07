#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int get_rssi(void)
{
	const char *wireless = "/proc/net/wireless";
	const char *interface = " mlan0";
	char *lineptr;
	int rssi = 0;
	size_t n;
	FILE *fp;

	fp = fopen(wireless, "r");
	if (fp == NULL) {
		//log_printf(LOG_ERROR, "unable to open (%s): %m\n", wireless);
		return 0;
	}

	while (true) {
		ssize_t len;
		char tmpstr[32], level[8];

		n = 0;
		lineptr = NULL;
		len = getline(&lineptr, &n, fp);
		if (len < 0)
			break;

		if (strncmp(lineptr, interface, strlen(interface)) != 0) {
			continue;
		}
		memset(level, 0, sizeof(level));
		sscanf(lineptr, "%31[^.].%7[^.]", tmpstr, level);
		printf("level: %d\n", atoi(level));

		break;
	}

	free(lineptr);
	fclose(fp);
	return rssi;
}

int main(char **argv, int argc)
{
	get_rssi();
	return 0;
}
