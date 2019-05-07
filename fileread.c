#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MIIO_DEVICE_CONFIG_FILE "/etc/miio_device.conf"


int main(void)
{
	FILE *fp;
	uint64_t did = 0;
	char key[16];
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char tmpbuf[64];


	const char *vendor = "testvendor";

	char *string_template = "{'method':'_otd.id', 'params':{'did':'%s', 'key':'%s', 'vendor': '%s'}}";

	fp = fopen(MIIO_DEVICE_CONFIG_FILE, "r");
	if (fp == NULL) {
		printf("%s:line%d, %m\n", __FILE__, __LINE__);
		return -1;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		printf("Retrieved line of length %zu :\n", read);
		printf("%s", line);

		if (line[0] == '#' || len < 5)
			continue;

		if (strncmp(line, "did", 3) == 0) {
			sscanf(line, "did=%Ld", (long long int *)&did);
			printf("did = 0x%x %lli\n", (unsigned int)did, (long long)did);
		}
	}
}
