#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

int main(){
  //create a socket file and make the connection
 int socket_fd = 0;

//socket creating
 socket_fd = socket(AF_INET, SOCK_STREAM, 0);
 if(socket_fd == -1){  //if error
  cerr << "Fail to create a socket.";
  return 0;
 }
 
//server  setting
 struct sockaddr_in server;
 bzero(&server, sizeof(server));
 server.sin_family = AF_INET;
 server.sin_addr.s_addr = inet_addr("127.0.0.1");
 server.sin_port = htons(2048);

//connection
 int err = -1;
 err = connect(socket_fd, (struct sockaddr*) &server, sizeof(server));
 if(err == -1){
  cerr << "Connection error.";
  return 0;
 }

  //using char but not string because of strcat and strcpy
 char Message_Received[128];
 char input[128];
 char output[256];
 bool login = false;
 recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
 memset(Message_Received, '\0', sizeof(Message_Received));

//login page
 while(login==false){
  memset(Message_Received, '\0', sizeof(Message_Received));
  memset(input, '\0', sizeof(input));
  memset(output, '\0', sizeof(output));
  char command[128];
  cin >> command;

 //case: register
  if(strcmp("REGISTER", command)==0){
   cout << "Register format: REGISTER#<Username> "<< endl;
   cin >> input;
   strcat(output, input);
   strcat(output, "\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
   cout << Message_Received;
  }

 //case: login
  else if(strcmp("LOGIN", command)==0){
   cout << "Login format: <Username>#<PortNum>"<< endl;
   cin >> input;
   strcat(output, input);
   strcat(output, "\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);

   //To check if the name is registered
   if(strstr(Message_Received, "AUTH_FAIL") == NULL){
     login = true;
     cout << Message_Received;
   }
   else{
     cout << Message_Received;
     cerr << "Invalid Entry, please try again." << endl;
   }
  }
  else{
     cerr << "Invalid Entry, please try again." << endl;
  }
 }

//login success, using List and Quit
 while(login==true){
  memset(Message_Received, '\0', sizeof(Message_Received));
  memset(input, '\0', sizeof(input));
  memset(output, '\0', sizeof(output));
  char command[128];
  cin >> command;
 
 //to call list
  if(strcmp("List", command)==0){
   strcpy(output, "List\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
   cout << Message_Received;
  }
 //to quit
  else if(strcmp("Exit", command)==0){
   strcpy(output, "Exit\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
   cout << Message_Received << endl;
   if(strstr(Message_Received, "Bye") != NULL){
      login = false;
   }
  }
  else{
    cerr << "Invalid Entry, please try again." << endl;
  }
 }
 return 0;
}