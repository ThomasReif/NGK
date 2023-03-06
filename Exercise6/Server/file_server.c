#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"

#define BUFSIZE_RX 1000
#define BUFSIZE_TX 1000

// Error handling
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Main program
int main(int argc, char *argv[])
{
    printf("Starting server...\n");

    int sockfd, newsockfd, portno;

    socklen_t clilen;
    uint8_t bufferRx[BUFSIZE_RX];
    uint8_t bufferTx[BUFSIZE_TX];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    FILE *fp;

    if (argc < 2)
    {
        error("ERROR usage: port");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    printf("Binding...\n");

    // Placere "n" zero-valued bytes
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // atoi converts character string to integer value
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Htons returnere en 16-bit TCP orderet 16-bit
    serv_addr.sin_port = htons(portno);

    // Fejl håndtering
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    printf("Listning... \n");
    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    char buf[256];
    char dataOut[BUFSIZE_TX] = {0};
        char dataIn[BUFSIZE_RX] = {0};
        const char *fileName;
        long fileSize;

    for (;;)
    {
        

        printf("Accept...\n");
        // New filpointer
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        // Fejl håndtering
        if (newsockfd < 0)
            error("ERROR on accept");
        else
            printf("Acceptet\n");

        // Ryd buffer
        bzero(dataIn, sizeof(dataIn));
        // // Ryd buffer out
        // bzero(bufferTx, sizeof(bufferTx));

        // Læs fra client
        // printf("Reading from client...\n");
        // Læs filnavn
        readTextTCP(newsockfd, dataIn, BUFSIZE_RX);
        // printf("Reading from client...\n");
        fileName = extractFileName(dataIn);

        printf("Request on Filename: %s\n", (char*)fileName);


        size_t bytes;
        size_t sendBytes;
        fp = fopen(fileName, "rb");
        if (fp == NULL)
        {
            printf("ERROR in opening file\n");
            fileSize = 0;
            snprintf(dataOut,BUFSIZE_TX, "%ld",fileSize);
            writeTextTCP(newsockfd, dataOut);
        }
        else
        {
            fileSize = getFilesize(fileName);
            snprintf(dataOut, BUFSIZE_TX, "%ld", fileSize);
            printf("\n%s %ld\n", fileName, fileSize);
            writeTextTCP(newsockfd, dataOut);

            char dataBUF[BUFSIZE_TX] = {0};
            bytes = fread(dataBUF, 1, sizeof(dataBUF), fp);
            long bytesSend = 0;

            while (bytes)
            {
                sendBytes = sendBytes + write(newsockfd, dataBUF, sizeof(dataBUF));
                bytes = fread(dataBUF, 1, sizeof(dataBUF), fp);
                //bytesSend += bytes;
                //printf("%ld\n", bytesSend);
            }
            printf("Data transfer completed\n\n");
            
        }
        

    }
    fclose(fp);
    close(newsockfd);

    close(sockfd);
    return 0;
}
