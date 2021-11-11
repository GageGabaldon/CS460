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

         // read a operation
         fgets(input, sizeof(input), stdin);

         if(input[0] == 'q')
         {
            char qi = 'q';
            printf("Shutting Down Everything\n");
            write(client_socket, &qi, sizeof(char));
            break;
         }

         // write to the server
         int i = 0;
         while (*(input + i))
         {
            // make the request to the server
            write(client_socket, input + i, sizeof(char));
            i++;
         }

         // get the result
         read(client_socket, &output, sizeof(double));

         // if the result is a non accepted operation
         if(output == -INFINITY)
         {
            int divideByZero = FALSE;
            for(int i = 0; i < strlen(input); i++)
            {
               if(input[i] == '0' && input[i - 2] == '/')
               {
                  printf("Cannot divide by 0\n");
                  divideByZero = TRUE;
               }
            }
            if(!divideByZero)
            {
               printf("Syntax Error\n");
            }
         }
         else
         {
            printf("Result: %f\n", output);
         }
      }

    return EXIT_SUCCESS;
}
