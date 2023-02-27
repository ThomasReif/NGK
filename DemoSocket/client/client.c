/* A simple client in the internet domain using TCP
The ip adresse and port number on server is passed as arguments
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm

Modified: Michael Alrøe -> Thomas Reifling
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "iknlib.h"

#define BUFSIZE 1000
#define file_client "/home/ase/NGK/Exercise6/test_picture.jpg"

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	printf("Starting client...\n");

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	// char buffer[BUFSIZE]; Bruger input
	char *buffer = argv[3]; // Input gennem terminal

	if (argc < 3)
		error("ERROR usage: "
			  "hostname"
			  ",  "
			  "port"
			  "");

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname(argv[1]);
	if (server == NULL)
		error("ERROR no such host");

	printf("Server at: %s, port: %s\n", argv[1], argv[2]);

	printf("Connected...\n");
	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	const char *fileName;
	long fileSize;

	while (1)
	{

		// CHOOSE FILE
		// File to be sent
		// printf("Please enter filepath and file: "); Bruger input
		// fgets(buffer, sizeof(buffer), stdin); Bruger input
		buffer[strcspn(buffer, "\n")] = 0;
		writeTextTCP(sockfd, buffer);

		// readTextTCP(sockfd,buffer,sizeof(buffer));

		fileName = extractFileName(buffer);
		fileSize = readFileSizeTCP(sockfd);

		printf("Client requestet: %s\n", fileName);
		if (fileSize == 0)
		{
			error("ERROR file doesn't exist\n");
			exit(1);
		}
		else
		{
			printf("%s: %ld bytes\n", fileName, fileSize);
		}

		// Download file
		FILE *fp;
		// Create file
		fp = fopen(fileName, "wb");
		if (fp == 0)
			error("ERROR opening file\n");
		else
			printf("File opened\n");

		// Read file in chunks of 1000
		int bytesRecieved = 0;
		int bytesToReceive = 0;
		int bytes = 0;
		while (1)
		{
			bzero(buffer, sizeof(buffer));
			printf("%i/%ld\n", bytesRecieved, fileSize);
			bytesToReceive = (fileSize - bytesRecieved) > 1000 ? 1000 : (fileSize - bytesRecieved);
			bytes = recv(sockfd, buffer, bytesToReceive, MSG_WAITALL);
			if (bytes <= 0)
				error("ERROR receiving data\n");

			fwrite(buffer, 1, bytes, fp);
			bytesRecieved = bytesRecieved + bytes;
			if (bytesRecieved == fileSize)
			{
				printf("%i/%ld \n\n>>Download complete!<<\n\n", bytesRecieved, fileSize);
				break;
			}
		}
		fclose(fp);
		printf("FileSize %ld\n", fileSize);
		printf("Filesize on local drive %ld\n", getFilesize(fileName));
		printf("Client closing...\n");
		close(sockfd);
		printf("Client closed...\n\n");

		return 0;
	}
}
