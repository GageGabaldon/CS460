#include "client.h"
#include <math.h>

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    char input[50];                    // buffer for user input
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    double output;

    // client name
    printf("Compute Client\n");

    // create an unnamed socket, and then name it
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // create addr struct
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    client_address.sin_port = htons(PORT);

    // connect to server socket
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }
    while(TRUE)
    {
      // simple message
         printf("Input a function to compute or exit with q\n");

         // read an integer
         fgets(input, sizeof(input), stdin);

         if(input[0] == 'q')
         {
            char qi = 'q';
            write(client_socket, &qi, sizeof(char));
            break;
         }

         int i = 0;
         while (*(input + i))
         {
            // make the request to the server
            write(client_socket, input + i, sizeof(char));
            i++;
         }

         // get the result
         read(client_socket, &output, sizeof(double));
         if(output == -INFINITY)
         {
            printf("Syntax Error\n");
         }
         else
         {
            printf("Result: %f\n", output);
         }
      }

    return EXIT_SUCCESS;
}
