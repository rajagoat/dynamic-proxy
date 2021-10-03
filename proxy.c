#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>     //hostent
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    //client_sock represents the incoming connection from the web browser, proxy_sock represents
    //the server-side socket of the proxy, and server_sock is the socket used to connect to the
    //remote server
    int client_sock, proxy_sock, server_sock, c, recv_status;
    struct sockaddr_in client, proxy, server;
    char client_message[5000], ip[100], server_reply[10000];
    char *hostname = "pages.cpsc.ucalgary.ca";
    struct hostent *he;
    struct in_addr **addr_list;
    char keywords[10][20] = {"", "", "", "", "", "", "", "", "", ""};

    //get IP address of remote server
    if ((he = gethostbyname(hostname)) == NULL)
    {
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (int i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(ip, inet_ntoa(*addr_list[i]));
    }

    printf("%s resolved to : %s\n", hostname, ip);

    //create socket for server-side proxy
    proxy_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy_sock == -1)
    {
        printf("Could not create socket.");
        return 1;
    }
    puts("Server-side proxy socket created.");

    proxy.sin_family = AF_INET;
    proxy.sin_addr.s_addr = INADDR_ANY;
    proxy.sin_port = htons(8786); //server-side proxy will use port 8786

    //bind server-side proxy to port
    if (bind(proxy_sock, (struct sockaddr *)&proxy, sizeof(proxy)) < 0)
    {
        puts("Bind failed.");
        return 1;
    }
    puts("Bind done.");

    while (1)
    {
        //listen
        listen(proxy_sock, 3);

        //update keywords array using keywords.txt file
        FILE *fp;

        fp = fopen("keywords.txt", "r");
        if (fp == NULL)
        {
            perror("File open failed.");
        }
        else
        {
            int i = 0;
            while (fgets(keywords[i], 20, fp) != NULL && i < sizeof(keywords) / sizeof(keywords[0]))
            {
                // puts(keywords[i]);
                i++;
            }
            fclose(fp);
        }

        printf("Here are your updated keywords:\n");
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            printf(keywords[i]);
        }

        //accept an incoming connection
        puts("Waiting for incoming connections...");
        c = sizeof(struct sockaddr_in);
        client_sock = accept(proxy_sock, (struct sockaddr *)&client, (socklen_t *)&c);
        if (client_sock < 0)
        {
            perror("Accept failed.");
            return 1;
        }
        puts("Connection accepted.");
        //receive message from client
        recv_status = recv(client_sock, client_message, 10000, 0);
        if (recv_status < 0)
        {
            perror("Error in receiving!");
            return 1;
        }

        if (strstr(client_message, "blocker") != NULL)
        {
            //if receiving connection from blocker, restart the loop to update keywords
            continue;
        }
        else if (strstr(client_message, "GET") != NULL)
        {
            //PARSING CLIENT MESSAGE (I.E. URL OF HTTP REQUEST) DONE HERE
            for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]) && strcmp(keywords[i], "") != 0 ; i++)
            {
                char keyword[20];
                memset(keyword, 0, strlen(keyword));
                strncpy(keyword, keywords[i], strlen(keywords[i])-1);
                printf("Keyword: %s\n", keyword);
                if (strstr(client_message, keyword) != NULL && strcmp(keyword, "") != 0)
                {
                    printf("\n\nThe following keyword was detected: %s\n\n", keyword);
                    strcpy(client_message, "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/error.html HTTP/1.0\r\n\r\n");
                    break;
                }
            }
            puts(client_message);

            //create socket for client-side proxy, i.e. to connect to remote server
            server_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (server_sock == -1)
            {
                printf("Could not create socket.");
            }
            puts("Client-side proxy socket created.");

            server.sin_addr.s_addr = inet_addr(ip);
            server.sin_family = AF_INET;
            server.sin_port = htons(80);

            //connect to remote server
            if (connect(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0)
            {
                puts("Connection error.");
                return 1;
            }
            puts("Connected\n");

            //forward client message to remote server
            if (send(server_sock, client_message, strlen(client_message), 0) < 0)
            {
                puts("Send failed.");
                return 1;
            }
            puts("Data sent\n");

            //receive reply from remote server
            while ((recv_status = recv(server_sock, server_reply, 4096, 0)) >= 0)
            {
                puts("Reply received\n");
                // puts(server_reply);
                //forward server reply to client
                if (send(client_sock, server_reply, recv_status, 0) < 0)
                {
                    puts("Write failed.");
                    return 1;
                }
                puts("Sent reply to web browser.");
                if (recv_status == 0)
                {
                    break;
                }
            }
            if (recv_status < 0)
            {
                puts("Receive failed.");
                return 1;
            }
        }
        close(client_sock);
        close(server_sock);
    }

    close(proxy_sock);

    return 0;
}