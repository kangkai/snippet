#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

static int get_localip(void)
{
	const char* google_dns_server = "8.8.8.8";
	int dns_port = 53;
	struct sockaddr_in serv;

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("Socket create error");
	}

	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(google_dns_server);
	serv.sin_port = htons(dns_port);

	int err = connect(sock, (const struct sockaddr *)&serv, sizeof(serv));

	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	getsockname(sock, (struct sockaddr *)&name, &namelen);

	close(sock);

	if(name.sin_addr.s_addr == 0)
		return -1;
	else
		return 0;
}

int
main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s, j;
	size_t len;
	ssize_t nread;
	char buf[BUF_SIZE];

	printf("get_localip() return: %d\n", get_localip());
	return;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
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
