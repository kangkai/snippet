#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

int
main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s, j;
	size_t len;
	ssize_t nread;
	char buf[BUF_SIZE];

	if (argc < 3) {
		fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	while (1) {
		int ret;

#if defined(__UCLIBC__)
#if ((__UCLIBC_MAJOR__ << 16) + (__UCLIBC_MINOR__ << 8) + (__UCLIBC_SUBLEVEL__) < 0x000922)
		/* uClibc 0.9.33.2 and before, res_init() is buggy, we
		 * need this res_nclose to set __res_sync = NULL before we
		 * call res_init(), otherwise, res_init() never re-read
		 * /etc/resolv.conf */
		res_nclose(NULL);
#endif
		printf("--------------\n");
		printf("_res.options: %08x\n", _res.options);
		if((ret = res_init()) != 0){ //hopefully this can fix getaddrinfo problem.
			printf("res_init error: %m\n");
			sleep(2);
			continue;
		}
		printf("_res.options after: %08x\n", _res.options);
		//_res.options |= (RES_DEBUG | RES_USEVC);

		printf("count: %d\n", _res.nscount);
		printf("addr: %s\n", inet_ntoa(_res.nsaddr_list[0].sin_addr));
#else
		printf("not in uclibc\n");
#endif

		/* Obtain address(es) matching host/port */

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = 0; /* Datagram socket */

		hints.ai_flags = 0;
		hints.ai_protocol = 0;          /* Any protocol */

		s = getaddrinfo(argv[1], argv[2], &hints, &result);
		if (s != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
			sleep(2);
			continue;
		}
		break;
	}

	/* getaddrinfo() returns a list of address structures.
	   Try each address until we successfully connect(2).
	   If socket(2) (or connect(2)) fails, we (close the socket
	   and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		s = getnameinfo(rp->ai_addr, rp->ai_addrlen, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
		if (s != 0) {
			printf("error in getnameinfo: %s\n", gai_strerror(s));
		} else
			printf("Trying ip: %s ...", buf);

		sfd = socket(rp->ai_family, rp->ai_socktype,
			     rp->ai_protocol);
		if (sfd == -1) {
			printf("fail\n");
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			printf("success\n");
		else
			printf("fail\n");

		close(sfd);
	}

	freeaddrinfo(result);           /* No longer needed */

	/* Send remaining command-line arguments as separate
	   datagrams, and read responses from server */

	for (j = 3; j < argc; j++) {
		len = strlen(argv[j]) + 1;
		/* +1 for terminating null byte */

		if (len + 1 > BUF_SIZE) {
			fprintf(stderr,
				"Ignoring long message in argument %d\n", j);
			continue;
		}

		if (write(sfd, argv[j], len) != len) {
			fprintf(stderr, "partial/failed write\n");
			exit(EXIT_FAILURE);
		}

		nread = read(sfd, buf, BUF_SIZE);
		if (nread == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		printf("Received %ld bytes: %s\n", (long) nread, buf);
	}

	exit(EXIT_SUCCESS);
}
