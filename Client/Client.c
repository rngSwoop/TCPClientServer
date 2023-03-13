/* 
#  Name:          Garrett King
#  Description:   TCP Client for message recorder
#  Date:          02/22/23
#  Specification: 
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Utilities.h"
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 50444
#define RECEIVE_BUFFER_SIZE 100
#define MESSAGE_BUFFER_SIZE 100
int main(int argc, char const *argv[]) 
{
  struct sockaddr_in server_endpoint;
  char message[MESSAGE_BUFFER_SIZE] = {0};
  char receive_buffer[RECEIVE_BUFFER_SIZE] = {0};
  printf("Connecting to server: %s:%d\n", SERVER_IP_ADDRESS, SERVER_PORT);
  int sock_desc = ConnectToTcpServer(&server_endpoint, AF_INET, SERVER_IP_ADDRESS, SERVER_PORT, SOCK_STREAM);
  int quit = -1;
  while (quit != 0)
  {
    //Receive message from server
    ReceiveMessageOverTcp(sock_desc, receive_buffer, RECEIVE_BUFFER_SIZE);
    ReadUserInput(receive_buffer, message, RECEIVE_BUFFER_SIZE);
    //Send user's message to server
    SendMessageOverTcp(sock_desc, message);
    quit = strcmp(message, "quit");
  }
  
  close(sock_desc);
  return 0;
}