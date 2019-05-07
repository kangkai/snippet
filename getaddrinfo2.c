/*
 * Usage: ./getaddrinfo2 host [port]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


#ifndef   NI_MAXHOST
#define   NI_MAXHOST 1025
#endif

/* Convert a struct sockaddr address to a string, IPv4 and IPv6
 * char str[INET6_ADDRSTRLEN]
 */
int get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
	switch(sa->sa_family) {
        case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
			  s, maxlen);
		break;

        case AF_INET6:
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
			  s, maxlen);
		break;

        default:
		strncpy(s, "Unknown AF", maxlen);
		return -1;
	}

	return 0;
}


int main(int argc, char **argv)
{
	struct addrinfo hints;
    struct addrinfo* result;
    struct addrinfo* res;
    int error;
    char *port = NULL;

    if (argc < 2) {
	    printf("Usage: %s hostname [port]\n", argv[0]);
	    return -1;
    }
    if (argc == 3)
	    port = argv[2];

    memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

    /* resolve the domain name into a list of addresses */
    error = getaddrinfo(argv[1], port, NULL, &result);
    if (error != 0) {   
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        }   
        exit(EXIT_FAILURE);
    }   

    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {   
        char hostname[NI_MAXHOST];
	char ipstr[INET6_ADDRSTRLEN];
	int len = sizeof(struct sockaddr); 
	int sockfd;


	if (res->ai_socktype == SOCK_STREAM) {
		printf("tcp...\n");
	} else if (res->ai_socktype == SOCK_DGRAM) {
		printf("udp...\n");
	} else
		printf("unknown...\n");
/*
        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
        if (error != 0) {
            fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname != '\0')
            printf("hostname: %s\n", hostname);

*/
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd == -1) {
			printf("error in create socket: %m\n");
			continue;
		}

//		if (res->ai_socktype == SOCK_STREAM &&
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
			printf("error in trying to connect socket: %m\n");
			continue;
		}

		/* getsockname, getpeername */
		getsockname(sockfd, res->ai_addr, &len);
		ipstr[0] = '\0';
		get_ip_str(res->ai_addr, ipstr, INET6_ADDRSTRLEN);
		printf("local for socket:%s\n", ipstr);

		getpeername(sockfd, res->ai_addr, &len);
		ipstr[0] = '\0';
		get_ip_str(res->ai_addr, ipstr, INET6_ADDRSTRLEN);
		printf("peer for socket:%s\n", ipstr);
		
		close(sockfd);


    }   

    freeaddrinfo(result);
    return 0;
}
