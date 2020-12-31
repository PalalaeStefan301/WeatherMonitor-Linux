#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sqlite3.h>
char buffer[4096] = "";

int callback(void *NotUsed, int argc, char **argv, char **azColName);
int main(int argc, char **argv)
{
	int fd = open("program.txt", O_CREAT | O_WRONLY | O_TRUNC);
	int change_status = 0;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	int sockfd, ret;
	struct sockaddr_in serverAddr;
	int newSocket;
	struct sockaddr_in newAddr;
	socklen_t addr_size;
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", atoi(argv[1]));

	if (listen(sockfd, 10) == 0)
	{
		printf("[+]Listening....\n");
	}
	else
	{
		printf("[-]Error in binding.\n");
	}
	rc = sqlite3_open("weather-info.db", &db);
	if (rc != SQLITE_OK)
	{

		fprintf(stderr, "Cannot open database: %s\n",
				sqlite3_errmsg(db));
		sqlite3_close(db);

		return 1;
	}
	while (1)
	{
		newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
		if (newSocket < 0)
		{
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if ((childpid = fork()) == 0)
		{
			close(sockfd);

			while (1)
			{
				recv(newSocket, buffer, 1024, 0);
				if (strcmp(buffer, ":exit") == 0)
				{
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
				else
				{
					if (strcmp(buffer, "lsregions") == 0)
					{
						printf("Listing regions...");
						char *sql = "SELECT * FROM regions;";
						rc = sqlite3_exec(db, sql, callback, buffer, &zErrMsg);
						if (rc != SQLITE_OK)
						{
							fprintf(stderr, "Failed to select data\n");
							fprintf(stderr, "SQL error: %s\n", zErrMsg);
							sqlite3_free(zErrMsg);
							sqlite3_close(db);
						}
					}
					else
					{
						if (strcmp(buffer, "change") == 0)
						{
							change_status = 1;
							strcpy(buffer, "Choose what");
						}
						else
						{

							if (strcmp(buffer, "regions,cities") == 0 )
							{
								printf("Listing regions...");
								char *sql = "SELECT * FROM ";
								strcat(sql, buffer);
								strcat(sql, " ;");
								rc = sqlite3_exec(db, sql, callback, buffer, &zErrMsg);
								if (rc != SQLITE_OK)
								{
									fprintf(stderr, "Failed to select data\n");
									fprintf(stderr, "SQL error: %s\n", zErrMsg);
									sqlite3_free(zErrMsg);
									sqlite3_close(db);
								}
								write(fd, buffer, sizeof(buffer));
							}
							
						}
					}
					printf("Client: %s\n", buffer);
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
				}
			}
		}
		else
		{
			continue;
		}
	}

	close(newSocket);
	close(fd);
	sqlite3_close(db);
	return 0;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName)
{

	NotUsed = 0;

	for (int i = 0; i < argc; i++)
	{
		strcat(buffer, azColName[i]);
		strcat(buffer, " = ");
		strcat(buffer, argv[i] ? argv[i] : "NULL");
		strcat(buffer, "\n");
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");

	return 0;
}