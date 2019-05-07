#include <stdio.h>
#include <string.h>

int main(void)
{
	char did[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x29,0xD2};
	char buf[8] = "1234567";
	buf[7] = '\0';
	int d = 10707; /* 0x29d3 */
	char tmpbuf[32];

	memset(tmpbuf, 0, 32);
	sprintf(tmpbuf, "%x", d);
	printf("tmpbuf: %s, tmpbuf[0]: %c\n", tmpbuf, tmpbuf[0]);

	printf("buf = %s\n", buf);
	printf("buf[0] = %c\n", buf[0]);

	printf("did[0] = 0x%02x\n", did[0]);

	return 0;
}
