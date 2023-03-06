#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(0); // fjernes hvis den ikke skal lukkes ved forkert input
}

int main(int argc, char *argv[])
{
    int sock, length, n;
    socklen_t fromlen;
    struct sockaddr_in server;
    struct sockaddr_in from;
    char buf[1024];

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(0);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        error("opening socket\n");
    }

    length = sizeof(server);

    bzero(&server, length);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&server, length) < 0)
    {
        error("binding\n");
    }

    fromlen = sizeof(struct sockaddr_in);

    while (1)
    {
        n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0)
        {
            error("Recvfrom\n");
        }

        FILE *fp;

        if (buf[0] == 'u' || buf[0] == 'U')
        {
            fp = fopen("/proc/uptime", "rb");
        }
        else if (buf[0] == 'l' || buf[0] == 'L')
        {
            fp = fopen("/proc/loadavg", "rb");
        }
        else if (fp == NULL)
        {
            printf("ERROR in opening file\n");
        }
        else
        {
            write(1, "Invalid command\n", 17);
            n = sendto(sock, "Invalid command\n", 17, 0, (struct sockaddr *)&from, fromlen);
        }
        bzero(buf, sizeof(buf));

        fread(buf, 1, sizeof(buf), fp);
        fclose(fp);

        n = sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, fromlen);

        if (n < 0)
        {
            error("sendto\n");
        }
    }
    return 0;
}
