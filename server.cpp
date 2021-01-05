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
#include <iostream>

#define SWITCH(S) \
    char *_S = S; \
    if (0)
#define CASE(S)                  \
    }                            \
    else if (strcmp(_S, S) == 0) \
    {                            \
        switch (1)               \
        {                        \
        case 1
#define BREAK \
    }
#define DEFAULT    \
    }              \
    else           \
    {              \
        switch (1) \
        {          \
        case 1

#define SWITCH1(S) \
    int _S = S;    \
    if (0)
#define CASE1(S)      \
    }                 \
    else if (_S == S) \
    {                 \
        switch (1)    \
        {             \
        case 1
#define BREAK1 \
    }
#define DEFAULT1   \
    }              \
    else           \
    {              \
        switch (1) \
        {          \
        case 1

char buffer[4096] = "";
std::string output;
int callback(void *NotUsed, int argc, char **argv, char **azColName);

int main(int argc, char **argv)
{
    int fd;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    int sockfd, ret;
    struct sockaddr_in serverAddr;
    int newSocket;
    struct sockaddr_in newAddr;
    socklen_t addr_size;
    pid_t childpid;
    std::string sql;

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
            int status = 0, status_s = 0;
            std::string aux;
            while (1)
            {
                recv(newSocket, buffer, 4096, 0);
                if (status != 0)
                {
                    SWITCH1(status)
                    {
                        CASE1(7) : sql.erase();
                        sql.append("UPDATE cities SET (city_name,region_id,temperature,humidity,uv_index,sunrise,sunset,wind)=(");
                        char *pch;
                        int it = 0;
                        pch = strtok(buffer, ", ");
                        while (pch != NULL)
                        {
                            if (it == 0)
                            {
                                sql.append("'");
                                sql.append(pch);
                                sql.append("',");
                            }
                            else
                            {
                                if (it == 5 || it == 6)
                                {
                                    sql.append("'");
                                    sql.append(pch);
                                    sql.append("',");
                                }
                                else
                                {
                                    if (it == 7)
                                    {
                                        sql.append(pch);
                                        sql.append(" ) WHERE city_name='");
                                        sql.append(aux);
                                        aux.erase();
                                        sql.append("';");
                                    }
                                    else
                                    {
                                        sql.append(pch);
                                        sql.append(",");
                                    }
                                }
                            }
                            pch = strtok(NULL, ", ");
                            it++;
                        }
                        printf("%s \n", sql.c_str());
                        sql.append(");");
                        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
                        strcpy(buffer, "City updated, well done");
                        status = 0;
                        BREAK1;

                        CASE1(8) : sql.erase();
                        sql.append("UPDATE regions SET (region_name,temperature,humidity,uv_index,sunrise,sunset,wind)=(");
                        char *pch;
                        int it = 0;
                        pch = strtok(buffer, ", ");
                        while (pch != NULL)
                        {
                            if (it == 0)
                            {
                                sql.append("'");
                                sql.append(pch);
                                sql.append("',");
                            }
                            else
                            {
                                if (it == 4 || it == 5)
                                {
                                    sql.append("'");
                                    sql.append(pch);
                                    sql.append("',");
                                }
                                else
                                {
                                    if (it == 6)
                                    {
                                        sql.append(pch);
                                        sql.append(" ) WHERE region_name='");
                                        sql.append(aux);
                                        aux.erase();
                                        sql.append("';");
                                    }
                                    else
                                    {
                                        sql.append(pch);
                                        sql.append(",");
                                    }
                                }
                            }
                            pch = strtok(NULL, ", ");
                            it++;
                        }
                        printf("%s \n", sql.c_str());
                        sql.append(");");
                        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
                        strcpy(buffer, "City updated, well done");
                        status = 0;
                        BREAK1;

                        CASE1(1) : sql.erase();
                        sql = "INSERT INTO cities(city_name,region_id,temperature,humidity,uv_index,sunrise,sunset,wind) VALUES (";
                        sql.append(buffer);
                        sql.append(");");
                        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
                        strcpy(buffer, "City inserted, well done");
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            strcpy(buffer, "Bad input, try again");
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                        }
                        status = 0;
                        BREAK1;
                        CASE1(2) : sql.erase();
                        sql = "INSERT INTO regions(region_name,temperature,humidity,uv_index,sunrise,sunset,wind) VALUES (";
                        sql.append(buffer);
                        sql.append(");");
                        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
                        strcpy(buffer, "Region inserted, well done");
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            strcpy(buffer, "Bad input, try again");
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                            printf("%s \n %s \n", buffer, sql.c_str());
                        }
                        status = 0;
                        BREAK1;
                        CASE1(3) : sql.erase();
                        sql = "DELETE FROM regions WHERE region_name='";
                        sql.append(buffer);
                        sql.append("';");
                        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
                        strcpy(buffer, "Region deleted, well done");
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            strcpy(buffer, "Bad input, try again");
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                        }
                        status = 0;
                        BREAK1;
                        CASE1(4) : sql.erase();
                        sql = "DELETE FROM cities WHERE city_name='";
                        sql.append(buffer);
                        sql.append("\';");
                        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
                        strcpy(buffer, "City deleted, well done");
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            strcpy(buffer, "Bad input, try again");
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                        }
                        status = 0;
                        BREAK1;
                        CASE1(5) : aux.erase();
                        aux.append(buffer);
                        sql.erase();
                        sql = "SELECT city_name,region_id,temperature,humidity,uv_index,sunrise,sunset,wind FROM cities WHERE city_name='";
                        sql.append(buffer);
                        sql.append("\';");
                        bzero(buffer, 4096);
                        printf("%s \n", sql.c_str());
                        rc = sqlite3_exec(db, sql.c_str(), callback, buffer, &zErrMsg);
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                        }

                        status = 7;
                        BREAK1;

                        CASE1(6) : aux.erase();
                        aux.append(buffer);
                        sql.erase();
                        sql = "SELECT region_name,temperature,humidity,uv_index,sunrise,sunset,wind FROM regions WHERE region_name='";
                        sql.append(buffer);
                        sql.append("\';");
                        bzero(buffer, 4096);
                        printf("%s \n", sql.c_str());
                        rc = sqlite3_exec(db, sql.c_str(), callback, buffer, &zErrMsg);
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                        }

                        status = 8;
                        BREAK1;

                    DEFAULT1:
                        printf("Message received: %s \n",buffer);
                        BREAK1;
                    }
                }
                else
                {
                    SWITCH(buffer)
                    {
                        CASE(".exit") : printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        break;
                        BREAK;

                        CASE("ls_regions") : bzero(buffer, 4096);
                        sql = "SELECT region_name FROM regions;";
                        rc = sqlite3_exec(db, sql.c_str(), callback, buffer, &zErrMsg);
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                        }
                        sql.erase();
                        BREAK;

                        CASE("ls_cities") : bzero(buffer, 4096);
                        sql = "SELECT city_name FROM cities;";
                        rc = sqlite3_exec(db, sql.c_str(), callback, buffer, &zErrMsg);
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", zErrMsg);
                            sqlite3_free(zErrMsg);
                            sqlite3_close(db);
                        }
                        sql.erase();
                        BREAK;

                        CASE("insert_region") : status = 2;
                        bzero(buffer, strlen(buffer));
                        strcpy(buffer, "Order is: region_name,temperature,humidity,uv_index,sunrise,sunset,wind");
                        BREAK;

                        CASE("insert_city") : status = 1;
                        bzero(buffer, strlen(buffer));
                        strcpy(buffer, "Order is: city_name,region_id,temperature,humidity,uv_index,sunrise,sunset,wind");
                        BREAK;

                        CASE("delete_region") : status = 3;
                        bzero(buffer, strlen(buffer));
                        memset(buffer, 0, strlen(buffer));
                        strcpy(buffer, "Which region");
                        BREAK;

                        CASE("delete_city") : status = 4;
                        bzero(buffer, strlen(buffer));
                        strcpy(buffer, "Which city");
                        BREAK;

                        CASE("update_city") : status = 5;
                        bzero(buffer, strlen(buffer));
                        strcpy(buffer, "Which city");
                        BREAK;

                        CASE("update_region") : status = 6;
                        bzero(buffer, strlen(buffer));
                        strcpy(buffer, "Which region");
                        BREAK;
                    DEFAULT:
                        printf("Message received: %s \n",buffer);
                        BREAK;
                    }
                }
                send(newSocket, buffer, strlen(buffer), 0);
                bzero(buffer, 4096);
                fflush(stdout);
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
        strcat(buffer, argv[i] ? argv[i] : "NULL");
        strcat(buffer, ",");
    }

    printf("\n");

    return 0;
}
