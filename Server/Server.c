/* 
#  Name:          Garrett King
#  Description:   TCP Server for message recorder
#  Date:          02/22/23
#  Specification:
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "Utilities.h"
#define TRUE 1
#define IP_ADDRESS "0.0.0.0"
#define SERVER_PORT 50444
#define RECEIVE_BUFFER_SIZE 1024

int main(int argc, char const *argv[]) 
{
  struct sockaddr_in server_endpoint;
  struct sockaddr_in client_endpoint;
  char receive_buffer[RECEIVE_BUFFER_SIZE] = {0};
  int server_sock_desc = CreateTcpServerSocket(&server_endpoint, AF_INET, IP_ADDRESS, SERVER_PORT, SOCK_STREAM);
  printf("Listening on Network Interface: %s Network Port: %d \n", IP_ADDRESS, SERVER_PORT);
 
  while (TRUE) 
  {
    printf("Waiting for client connection...\n");
   
    int client_sock_desc = AcceptTcpConnection(server_sock_desc, &client_endpoint);
    char *client_ip = inet_ntoa(client_endpoint.sin_addr);
    printf("Accepted connection: %s:%d\n\n", client_ip, ntohs(client_endpoint.sin_port));
    int quit = -1;
    //Invite client to send a message
    SendMessageOverTcp(client_sock_desc, "Would you like to send a message (y/n)? ");
    
	while (quit != 0)
    {
      //Receive message from client
      int byte_count = ReceiveMessageOverTcp(client_sock_desc, receive_buffer, RECEIVE_BUFFER_SIZE);
	  
      //Client responds 'n'
      if(!strcmp(receive_buffer, "n")) {
        
        //Print new line and exit while loop
        printf("\n");
        quit = 0;
		
      //Client responds 'y'
      } else if (!strcmp(receive_buffer, "y")) {
	  
        //Prompt client to record message
        char buf[] = "Enter message: ";
        SendMessageOverTcp(client_sock_desc, buf);
		
        //Receive client response
        byte_count = ReceiveMessageOverTcp(client_sock_desc, receive_buffer, RECEIVE_BUFFER_SIZE);
        printf("Client message received: %s\n", receive_buffer);
		
        //Convert message to ASCII text file prepended with
        //  - Date-Time Stamp
        //  - Client IP address
        char file_name[] = "data.txt";
        write_message(file_name, client_ip, receive_buffer);
        
        //Alert client of message success
        char status[] = "Server status message: STATUS_MESSAGE_SUCCESS\n\n";
        SendMessageOverTcp(client_sock_desc, status);
        
        quit = strcmp(receive_buffer, "quit");
		
        //Prompt client to send another message
        char goAgain[] = "Would you like to send a message (y/n)? ";
        SendMessageOverTcp(client_sock_desc, goAgain);
      
      }
	  
	  //Invalid client response
      else {
        char err[] = "Invalid response...\nWould you like to send a message (y/n)? ";
		SendMessageOverTcp(client_sock_desc, err);
      }
	  
    }
      
    close(client_sock_desc);
  }
  close(server_sock_desc); //Will never reach this point.
  return 0;
}

int write_message(char* filename, char* client_ip_addr, char* message);

int write_message(char* filename, char* client_ip_addr, char* message)
{
  time_t t = time(NULL);
  
  //printf("\n Current date and time is : %s", ctime(&t));
  FILE* fptr = fopen(filename, "a");
  
  fprintf(fptr, "%s", ctime(&t));
  
  fprintf(fptr, "%s\n", client_ip_addr);
  
  fprintf(fptr, "%s\n\n", message);
  fclose(fptr);
  return 0;
}