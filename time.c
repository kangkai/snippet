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

static unsigned int get_micro_second2(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	printf("sec: %llu, sec*1000000: %llu:%u, usec: %llu\n", (unsigned long long)tv.tv_sec, (unsigned long long)tv.tv_sec * 1000000, (unsigned int)tv.tv_sec * 1000000, (unsigned long long)tv.tv_usec);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

unsigned int get_micro_second(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

unsigned int timestamp(void)
{
	return get_micro_second() / 1000000;
}

uint64_t right_get_micro_second2(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

uint64_t right_get_micro_second(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

uint64_t right_timestamp(void)
{
	return right_get_micro_second() / 1000000;
}

int main(int argc, char **argv)
{
	printf("get_micro_second2: %u\n", get_micro_second2());
	printf("get_micro_second: %u\n", get_micro_second());
	printf("timestamp: %u\n", timestamp());

	printf("right_get_micro_second2: %llu\n", (unsigned long long)right_get_micro_second2());
	printf("right_get_micro_second: %llu\n", (unsigned long long)right_get_micro_second());
	printf("right_timestamp: %llu\n", (unsigned long long)right_timestamp());

	return 0;
}
