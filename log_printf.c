#include <stdio.h>
#include <stdarg.h>
#include <time.h>

void myprintf(const char *fmt, ...)
{
	char buf[80];
	time_t now = time(NULL);
	struct tm *p = localtime(&now);
	strftime(buf, 80, "[%Y%m%d %H:%M:%S]", p);

	va_list ap;
	va_start(ap, fmt);
	printf("%s ", buf);
	vprintf(fmt, ap);
	va_end(ap);
}

int main(int argc, char **argv)
{
	int a = 10;
	myprintf("hello world.\n");
	myprintf("hi a: %d\n", a);
	return 0;
}

