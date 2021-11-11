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
// for calculations assume negative signs are next to numbers and that their is a space between
// the numbers and operations 10 / 2 not 10/2 might still work but is an assumption
void ComputeClient(int socket, int id)
{
   char input;
   char string[50];
   int index = 0;
   int keep_going = TRUE;
   int NotDone = TRUE;

   // read a input and compute while client is still computing
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
         if(string[0] == 'q')
         {
            printf("Shutting Down Server\n");
            exit(EXIT_SUCCESS);
         }
      }
      if(!NotDone)
      {
         break;
      }

      // parse the string for input
      double output = parseString(string, index);

      // write to the client
      write(socket, &output, sizeof(double));

      printf("Operation: %sOutput: %f\n", string, output);
   }

   // cleanup
   if (close(socket) == -1) {
      perror("Error closing socket");
      exit(EXIT_FAILURE);
   }
}

// does the calcuations with two numbers for most operations and just num2
// for sqrt this is where we check if the numbers are valid or syntax is.
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
      if(num2 < 0)
      {
         printf("Cannot take a sqrt of a negative number\n");
         return -INFINITY;
      }
      return sqrt(num2);
   }
}

// this is where we parse the string and get num1, num2, and operation to be performed
double parseString(char *input, int index)
{
   // see number
   char firstNum[20];
   char secondNum[20];
   int firstIndex = 0;
   int secondIndex = 0;
   char operation = 'E';
   char *ret;
   int first = 1;
   int second = 0;
   int seenNeg = FALSE;
   double num1;
   double num2;

   // check if its a sqrt to use only one number
   ret = strstr(input, "sqrt");
   if(ret)
   {
      first = 0;
      operation = 's';
   }

   // iterate through the string
   for (int i = 0; i < index; i++)
   {
      // check if the char is a digit or is a . stores in first num else second num
      if((isdigit(input[i]) > 0 && first == 1) || input[i] == '.')
      {
         // check if num is negative
         if(checkNeg(input, i))
         {
            firstNum[firstIndex] = '-';
            firstIndex++;
         }
         firstNum[firstIndex] = input[i];
         firstIndex++;
      }
      else if((isdigit(input[i]) > 0 && first == 0) || input[i] == '.')
      {
         if(checkNeg(input, i))
         {
            secondNum[secondIndex] = '-';
            secondIndex++;
         }
         secondNum[secondIndex] = input[i];
         secondIndex++;
      }

      // this is used to check for what operations and to switch to using secondNum
      if(input[i] == '/')
      {
         operation = '/';
         first = 0;
         firstNum[firstIndex] = '\0';
      }
      // have to differentate - the operator and - the sign
      else if (input[i] == '-' && !seenNeg && operation != 's' && input[0] != '-')
      {
         operation = '-';
         first = 0;
         firstNum[firstIndex] = '\0';
         seenNeg = TRUE;
      }
      else if(input[i] == '^')
      {
         operation = '^';
         first = 0;
         firstNum[firstIndex] = '\0';

      }
      else if(input[i] == '+')
      {
         operation = '+';
         first = 0;
         firstNum[firstIndex] = '\0';
      }
      else if (input[i] == '*')
      {
         operation = '*';
         first = 0;
         firstNum[firstIndex] = '\0';

      }
   }
   secondNum[secondIndex] = '\0';

   // if operation is not available
   if(operation == 'E')
   {
      printf("Syntax Error\n");
      return -INFINITY;
   }

   // convert to float
   num2 = atof(secondNum);
   if(operation != 's')
   {
      num1 = atof(firstNum);
   }
   else
   {
      num1 = 0;
   }
   printf("FirstNum %f, SecondNum %f Operation, %c\n", num1, num2, operation);

   return do_calc(num1, num2, operation);
}

// checks if the number is negative or not assumes negative sign is last char
// before number
int checkNeg(char *string, int i)
{
   if(string[i - 1] == '-')
   {
      return TRUE;
   }
   return FALSE;
}