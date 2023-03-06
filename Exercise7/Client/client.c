#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(const char *);

int main(int argc, char *argv[])
{
    int sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    char buf[256];

    if (argc != 4)
    {
        printf("Usage: server port\n");
        exit(1);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        error("Socket\n");
    }

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == 0)
    {
        error("Unknown host\n");
    }

    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length = sizeof(struct sockaddr_in);

    bzero(buf, sizeof(buf));
    n = sendto(sock, argv[3], strlen(argv[3]), 0, (const struct sockaddr *)&server, length);
    if (n < 0)
    {
        error("Sendto");
    }
    n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &length);
    if (n < 0)
    {
        error("recvfrom");
    }

    write(1, "God an ack: \n", 12);

    write(1, buf, n);

    close(sock);

    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}