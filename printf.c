#include <stdio.h>

int main(int argc, char **argv)
{
	char sarray[] = {'h', 'e', 'l', 'l', 'o'};
	printf("%s(), %d: %.*s\n", __func__, __LINE__, 3, sarray);
	return 0;
}
