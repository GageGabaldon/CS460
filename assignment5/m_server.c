#include "m_server.h"
#include <pthread.h>

/************************************************************************
 * MAIN
 ************************************************************************/
// number of clients
pthread_mutex_t lock;
int server_socket;                 // descriptor of server socket
struct sockaddr_in server_address; // for naming the server's listening socket

int main(int argc, char** argv) {
    // sent when ,client disconnected
    signal(SIGPIPE, SIG_IGN);
    char input;

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

    pthread_mutex_init(&lock, NULL);

    // starting the server
    printf("Starting Server ...\n");
    for (int i = 0; i < MAX_THREADS ; i++) {
      pthread_t newThread;
      pthread_create(&newThread, NULL, handle_client, (void *)&i);
      pthread_detach(newThread);
    }

    // keep the threads running
    while(TRUE)
    {

   }
}


/************************************************************************
 * handle client
 ************************************************************************/
// thread function to handle multiple requests
void *handle_client(void *id) {
    int *id_pointer = (int *)id;
    int realID = *id_pointer;
    int client_socket;

    while(TRUE)
    {
      // lock while accepting clients to prevent
      //pthread_mutex_lock(&lock);
      if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
          perror("Error accepting client");
      }
      printf("Creating connection on socket: %d\n", client_socket);
      //pthread_mutex_unlock(&lock);

      readWrite(client_socket, realID);
    }

    // should never get here
    return NULL;
}

void readWrite(int socket, int id)
{
   int input;

   // read the int from data stream
   read(socket, &input, sizeof(int));

   // calcuate the 3A + 1 algorithm
   int output = three_a_plus_one(input);

   // transmit the nunmber computed
   write(socket, &output, sizeof(int));

   printf("THREAD %d: Given number: %d the output is %d\n", id, input, output);
   printf("THREAD %d: Closed socket %d\n", id, socket);
   sleep(.5);
   // cleanup
   if (close(socket) == -1) {
      perror("Error closing socket");
      exit(EXIT_FAILURE);
   }
}


/* ******************************************************* */
/* three_a_plus_one() - nonrecursive                       */
/* ******************************************************* */
int three_a_plus_one(int input)
{
    int counter = 0;
    int current = input;

    while (current != 1)
    {
        counter++;
        if (current % 2) {
            current = (current*3) + 1;
        }
        else {
            current >>= 1;
        }
    }
    return counter;
}


/* ******************************************************* */
/* three_a_plus_one_rec() - recursive                          */
/* ******************************************************* */
int three_a_plus_one_rec(int number) {
    int new_number;

    if (number == 1) {
        return 0;
    }

    if (number % 2) {
        new_number = 3 * number + 1;
    } else {
        new_number = number / 2;
    }

    return 1 + three_a_plus_one(new_number);
}