#include "server.h"

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    char c;

    printf("Time client\n");

    // create an unnamed socket, and then name it
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("Error creating socket");
      return -1;
    }

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(PORT);
    client_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);


    // connect to server socket
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address))) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }

    while (TRUE) {
        printf("Connected ...\n");

        while(c != '*') {
            // read char by char
            read(client_socket, &c, sizeof(char));
            printf("%c", c);
        }
        printf("\n");
        close(client_socket);
        printf("\nDone!\n");
        exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
}

