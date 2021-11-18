#include "client.h"
#include <math.h>

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    char input[50];                    // buffer for user input
    int client_socket;                  // client side socket
    struct sockaddr_in server_address;  // client socket naming struct
    double output;
    ssize_t bytes_recieved;
    socklen_t server_addr_length = sizeof(server_address);

    // client name
    printf("Compute Client\n");

    // create an unnamed socket, and then name it
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    // create addr struct
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_address.sin_port = htons(PORT);

    while(TRUE)
    {
      // simple message
         printf("Input a function to compute or exit with q:\n");

         // read a operation
         fgets(input, sizeof(input), stdin);
         if(input[0] == 'q')
         {
            char qi = 'q';
            printf("Shutting Down Everything\n");
            if(sendto(client_socket,
               (const char *)&input,
               strlen(input) + 1,
               0,
               (struct sockaddr *)&server_address,
               sizeof(server_address)) == -1)
               {
                  perror("sendto");
               }
            break;
         }

         // write to the server
         if(sendto(client_socket,
                  (const char *)&input,
                  strlen(input) + 1,
                  0,
                  (struct sockaddr *)&server_address,
                  sizeof(server_address)) == -1)
         {
               perror("sendto");
         }

         if((bytes_recieved = recvfrom(client_socket,
                  (double *)&output,
                  (size_t)sizeof(output),
                  MSG_WAITALL,
                  (struct sockaddr *)&server_address,
                  &server_addr_length
               )) == -1)
         {
            perror("recvfrom");
         }

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
