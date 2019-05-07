#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>

int lock_fd = -1;
#define LOCK_FILE "/tmp/alockfile"


int mylock()
{
	int ret;
	ret = flock(lock_fd, LOCK_EX);
	if (ret < 0)
		perror("flock");
	return ret;
}

int myunlock()
{
	int ret;
	ret = flock(lock_fd, LOCK_UN);
	if (ret < 0)
		perror("flock");

	return ret;
}

static void *tf (void *a)
{
	fprintf(stderr, "in thread 1, before lock\n");
	mylock();
	/* This call should never return.  */
	return a;
}


int
do_test (void)
{
  pthread_t th;

  fprintf(stderr, "in thread 0, before lock\n");
  mylock();
  fprintf(stderr, "in thread 0, after lock\n");

  if (pthread_create (&th, NULL, tf, NULL) != 0)
    {
      write (2, "create failed\n", 14);
      _exit (1);
    }

  pthread_join (th, NULL);

  fprintf(stderr, "join returned");

  return 0;
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);

	lock_fd = open(LOCK_FILE, O_CLOEXEC | O_CREAT);
	if (lock_fd == -1) {
		perror("open");
		exit(-1);
	}
	do_test();
	return 0;
}
