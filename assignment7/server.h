/************************************************************************
 * libraries
 ************************************************************************/
// should always be there ...
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// socket/bind/listen/accept
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>


// read/write/close
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>

#include <ctype.h>
#include <string.h>

/************************************************************************
 * function prototype declarations
 ************************************************************************/
void ComputeClient(int id);
double do_calc(double num1, double num2, char operation);
double parseString(char *input, int index);
int checkNeg(char *string, int i);

/************************************************************************
 * preprocessor directives
 ************************************************************************/
#define SERVER_ADDR "127.0.0.1" // loopback ip address
#define PORT 23657              // port the server will listen on

// I cannot let go of the old-fashioned way :) - for readability ...
#define FALSE false
#define TRUE !false

# define NUM_CONNECTIONS 1