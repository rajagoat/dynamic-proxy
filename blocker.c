#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <stdlib.h>

/*
blocker will handle all updating of keywords, which gets sent to keywords.txt.
Once each update is complete, it will send a "blocker" message to proxy to let
the proxy know.
*/

int main(int argc, char *argv[])
{
    int sock, recv_status;
    struct sockaddr_in proxy;
    char message[1000], proxy_reply[1000], option[20];
    char keywords[10][20] = {"", "", "", "", "", "", "", "", "", ""};

    //create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket.");
        return 1;
    }
    puts("Socket created.");

    proxy.sin_addr.s_addr = inet_addr("136.159.5.25");
    proxy.sin_family = AF_INET;
    proxy.sin_port = htons(8786);

    //reads into keywords file to get most up-to-date set of keywords
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

    printf("Here are your current keywords:\n");
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        printf(keywords[i]);
        /* if (strcmp(keywords[i], "") != 0)
        {
            printf("\n");
        } */
    }
    // display_keywords(keywords, sizeof(keywords) / sizeof(keywords[0]));

    printf("What would you like to do?\n");
    printf("Options: \n");
    printf("    add - add keyword\n");
    printf("    delete - delete keyword\n");
    printf("    nothing - exit program\n");
    scanf("%s", option);

    if (strcmp(option, "add") == 0)
    {
        printf("What keyword would you like to add?\n");
        scanf("%s", message);
        strcat(message, "\n");
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            printf("Keyword: %s\n", keywords[i]);
            printf("Message: %s\n", message);
            if (strcmp(keywords[i], "") == 0)
            {
                strcpy(keywords[i], /* strcat( */ message /* , "\n") */);
                printf("Keyword successfully added.\n");
                break;
            }
            else if (strcmp(keywords[i], message) == 0)
            {
                printf("Keyword already exists.\n");
                break;
            }
            if (i == sizeof(keywords) / sizeof(keywords[0] - 1))
            {
                printf("Too many keywords, could not add.\n");
            }
        }

        // display_keywords(keywords, sizeof(keywords) / sizeof(keywords[0]));
        FILE *fp;
        fp = fopen("keywords.txt", "w");

        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            if (strcmp(keywords[i], "") != 0)
            {
                fputs(keywords[i], fp);
            }
        }
        fclose(fp);

        printf("Here are your updated keywords:\n");
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            printf(keywords[i]);
        }

        //connect to proxy
        if (connect(sock, (struct sockaddr *)&proxy, sizeof(proxy)) < 0)
        {
            perror("Connect failed.");
            return 1;
        }
        puts("Connected.");

        memset(message, 0, sizeof(message));
        strcpy(message, "blocker");

        if (send(sock, message, strlen(message), 0) < 0)
        {
            puts("Send failed.");
            return 1;
        }
        puts("Data sent\n");
    }
    else if (strcmp(option, "delete") == 0)
    {
        printf("What keyword would you like to delete?\n");
        scanf("%s", message);
        strcat(message, "\n");
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            printf("Keyword: %s\n", keywords[i]);
            printf("Message: %s\n", message);
            if (strcmp(keywords[i], message) == 0)
            {
                memset(keywords[i], 0, strlen(keywords[i]));
                printf("Keyword successfully deleted.\n");
                break;
            }
            if (i == sizeof(keywords) / sizeof(keywords[0]) - 1) {
                printf("Keyword does not exist.\n");
            }
        }

        FILE *fp;
        fp = fopen("keywords.txt", "w");

        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            if (strcmp(keywords[i], "") != 0)
            {
                fputs(keywords[i], fp);
            }
        }
        fclose(fp);

        printf("Here are your updated keywords:\n");
        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            printf(keywords[i]);
        }

        //connect to proxy
        if (connect(sock, (struct sockaddr *)&proxy, sizeof(proxy)) < 0)
        {
            perror("Connect failed.");
            return 1;
        }
        puts("Connected.");

        memset(message, 0, sizeof(message));
        strcpy(message, "blocker");

        if (send(sock, message, strlen(message), 0) < 0)
        {
            puts("Send failed.");
            return 1;
        }
        puts("Data sent\n");
    }
    else if (strcmp(option, "nothing") == 0)
    {
        printf("Goodbye.\n");
        close(sock);
        return 0;
    }
    else
    {
        printf("Invalid command. Program will close.\n");
    }

    close(sock);

    return 0;
}