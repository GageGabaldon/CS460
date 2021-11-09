#include "server.h"
#include <math.h>

/************************************************************************
 * MAIN
 ************************************************************************/
int main(int argc, char** argv) {
   int server_socket;                 // descriptor of server socket
   struct sockaddr_in server_address; // for naming the server's listening socket
   int client_socket;

    // sent when ,client disconnected
    signal(SIGPIPE, SIG_IGN);

    // create unnamed network socket for server to listen on
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // name the socket (making sure the correct network byte ordering is observed)
    server_address.sin_family      = AF_INET;           // accept IP addresses
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // accept clients on any interface
    server_address.sin_port        = htons(PORT);       // port to listen on

    // binding unnamed socket to a particular port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // listen for client connections (pending connections get put into a queue)
    if (listen(server_socket, NUM_CONNECTIONS) == -1) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    printf("Starting Server ...\n");
    // keep the threads running
    while(TRUE)
    {
      if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
          perror("Error accepting client");
      }
      printf("Creating connection on socket: %d\n", client_socket);
      ComputeClient(client_socket, 1);
   }
}

void ComputeClient(int socket, int id)
{
   char input;
   char string[50];
   int index = 0;
   int keep_going = TRUE;
   int NotDone = TRUE;
   while(NotDone)
   {
      index = 0;
      memset(string, ' ', 50);
      while (keep_going)
      {
         // read char from client
         switch (read(socket, &input, sizeof(char)))
         {
            case 0:
               keep_going = FALSE;
               string[index] = '\0';
               printf("Finished reading Stream...\n");
               NotDone = FALSE;
               break;
            case -1:
               perror("Error reading from network!\n");
               keep_going = FALSE;
               break;
            default:
               string[index] = input;
               index++;

         }
         if(input == '\n')
         {
            string[index] = '\0';
            break;
         }
         if(input == 'q')
         {
            exit(EXIT_SUCCESS);
         }
      }
      if(!NotDone)
      {
         break;
      }

      double output = parseString(string, index);
      write(socket, &output, sizeof(double));

      printf("THREAD %d: Operation: %s \n the output is %f\n", id, string, output);
   }

   // cleanup
   if (close(socket) == -1) {
      perror("Error closing socket");
      exit(EXIT_FAILURE);
   }
}

double do_calc(double num1, double num2, char operation)
{
   if(operation == '/')
   {
      if(num2 == 0)
      {
         printf("Cannot Divide by Zero\n");
         return -INFINITY;
      }
      return num1 / num2;
   }
   else if (operation == '-')
   {
      return num1 - num2;
   }
   else if(operation == '^')
   {
      return pow(num1, num2);
   }
   else if(operation == '+')
   {
      return num1 + num2;
   }
   else if (operation == '*')
   {
      return num1 * num2;
   }
   else if(operation == 's')
   {
      if(num1 < 0)
      {
         printf("Cannot take a sqrt of a negative number\n");
         return -INFINITY;
      }
      return sqrt(num1);
   }
}

double parseString(char *input, int index)
{
   // see number
   char firstNum[20];
   char secondNum[20];
   int firstIndex = 0;
   int secondIndex = 0;
   char operation;
   int first = 1;
   int second = 0;
   double num1;
   double num2;

   for (int i = 0; i < index; i++)
   {
      if(isdigit(input[i]) > 0 && first == 1)
      {
         firstNum[firstIndex] = input[i];
         firstIndex++;
      }
      else if(isdigit(input[1]) > 0 && second == 1)
      {
         secondNum[secondIndex] = input[i];
         secondIndex++;
      }

      if(input[i] == '/')
      {
         operation = '/';
         first = 0;
         second = 1;
         firstNum[firstIndex] = '\0';
      }
      else if (input[i] == '-')
      {
         operation = '-';
         first = 0;
         second = 1;
         firstNum[firstIndex] = '\0';

      }
      else if(input[i] == '^')
      {
         operation = '^';
         first = 0;
         second = 1;
         firstNum[firstIndex] = '\0';

      }
      else if(input[i] == '+')
      {
         operation = '+';
         first = 0;
         firstNum[firstIndex] = '\0';
         second = 1;
      }
      else if (input[i] == '*')
      {
         operation = '*';
         first = 0;
         second = 1;
         firstNum[firstIndex] = '\0';

      }
      else if(input[i] == 's')
      {
         operation = 's';
         first = 0;
         second = 1;
         firstNum[firstIndex] = '\0';
      }
   }
   secondNum[secondIndex] = '\0';

   num1 = atoi(firstNum);
   if(operation != 's')
   {
      num2 = atoi(secondNum);
   }
   else
   {
      num2 = 1;
   }

   return do_calc(num1, num2, operation);
}