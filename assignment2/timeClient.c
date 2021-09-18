#include "timeClient.h"

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    int time_socket;                  // time side socket
    struct sockaddr_in time_server_address;  // time server naming struct
    struct hostent *host;
    char c;
    char* time_server_ip;

    printf("Time client\n");

    // create an unnamed socket, and then name it
    if((time_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("Error creating socket");
      return -1;
    }

    host = gethostbyname(SERVER_ADDR);
    time_server_ip = inet_ntoa(*((struct in_addr*)
                                 host->h_addr_list[0]));
    // server address
    time_server_address.sin_family = AF_INET;
    time_server_address.sin_port = htons(PORT);
    time_server_address.sin_addr.s_addr = inet_addr(time_server_ip);

    // connect to server socket
    if (connect(time_socket, (struct sockaddr *)&time_server_address, sizeof(time_server_address))) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }

    while (TRUE) {
        printf("Connected ...\n");

        while(c != '*') {
            // read char by char
            read(time_socket, &c, sizeof(char));
            printf("%c", c);
        }
        printf("\n");
        close(time_socket);
        printf("\nDone!\n");
        exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
}

