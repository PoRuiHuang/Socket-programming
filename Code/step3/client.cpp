#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>

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
 server.sin_family = PF_INET;
 server.sin_addr.s_addr = inet_addr("127.0.0.1");
 server.sin_port = htons(3078);

//connection
 int err = -1;
 err = connect(socket_fd, (struct sockaddr*) &server, sizeof(server));
 if(err == -1){
  cerr << "Connection error.\n";
  return 0;
 }

  //using char but not string because of strcat and strcpy
 char Message_Received[128];
 char input[128];
 char output[256];
 bool login = false;
 recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
  cout << Message_Received; 
 memset(Message_Received, '\0', sizeof(Message_Received));

//login page
 while(login==false){
  memset(Message_Received, '\0', sizeof(Message_Received));
  memset(input, '\0', sizeof(input));
  memset(output, '\0', sizeof(output));

  cerr << "Type \"LOGIN\" to login, type \"REGISTER\" to register." << endl;
  char command[128];
  cin >> command;

 //case: register
  if(strcmp("REGISTER", command)==0){
   cerr << "Register format: REGISTER#<Username> "<< endl;
   cin >> input;
   strcat(output, input);
   strcat(output, "\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
   cerr << Message_Received;
  }

 //case: login
  else if(strcmp("LOGIN", command)==0){
   cerr << "Login format: <Username>#<PortNum>"<< endl;
   cin >> input;
   strcat(output, input);
   strcat(output, "\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
   memset(input, '\0', sizeof(input));
   memset(output, '\0', sizeof(output));

   //To check if the name is registered
   if(strstr(Message_Received, "AUTH_FAIL") == NULL){
     login = true;
     cerr << Message_Received;
   }
   else{
     cerr << Message_Received;
     cerr << "Invalid Entry, please try again." << endl;
   }
  }
  else{
     cerr << "Invalid Entry, please try again." << endl;
  }
 }

//login success, using List and Quit
 while(login==true){
//  int err = -1;
//  err = connect(socket_fd, (struct sockaddr*) &server, sizeof(server));
//  if(err == -1){
//     cerr << "Connection error.\n";
//     return 0;
//  }
  memset(Message_Received, '\0', sizeof(Message_Received));
  memset(input, '\0', sizeof(input));
  memset(output, '\0', sizeof(output));
  cout << "----------------------------------------------------------------------------"<< endl;
  cout << "Type \"List\" to see online list,\ntype \"Transaction\" to transaction,\ntype \"Wait\" to wait for transaction,\ntype \"Exit\" to quit." << endl;
  cout << "----------------------------------------------------------------------------"<< endl;
  char command[128];
  cin >> command;
 
 //to call list
  if(strcmp("List", command)==0){
   strcpy(output, "List\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
   cerr << Message_Received;
  }
  
  //----------------------------------------------------------------------------------------
  else if(strcmp("Transaction", command)==0){
     int status;
     char ip_address[] = {};
     int portNumber = 0;
            if(fork() == 0){
                char request_user[100];
                memset(Message_Received, '\0', sizeof(Message_Received));
                strcpy(input, "TRANS#");
                cout << "Please enter the user you want to transact with(please make sure he is online):";
                cin >> request_user;
                strcat(input, request_user);
                strcat(input, "\n"); //end of input
                send(socket_fd, input, sizeof(input), 0);
                recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
                
                if(strstr(Message_Received, "Transaction fail, please check") != NULL)
                    cout << "No such user!" << endl;
                else
                {
                    // cout << Message_Received ;
                    strcpy(ip_address, strtok(Message_Received, "#"));
                    portNumber = atoi(strtok(NULL, "#"));                
                    // cout << portNumber;
                cout << "Please enter the transaction amount:";
                char money[10];
                cin >> money;

                int sock_fd2 = 0;
                sock_fd2 = socket(AF_INET , SOCK_STREAM , 0);

                if (sock_fd2 == -1){
                    cout << "Fail to create a socket.";
                    return 0;
                }

                struct sockaddr_in info2;
                bzero(&info2, sizeof(info2));
                info2.sin_family = PF_INET;
                info2.sin_addr.s_addr = inet_addr("127.0.0.1");
                info2.sin_port = htons(portNumber);

//              struct sockaddr_in server;
//  bzero(&server, sizeof(server));
//  server.sin_family = AF_INET;
//  server.sin_addr.s_addr = inet_addr("127.0.0.1");
//  server.sin_port = htons(3018);

// //connection
//  int err = -1;
//  err = connect(socket_fd, (struct sockaddr*) &server, sizeof(server));
//  if(err == -1){
//   cerr << "Connection error.\n";
//   return 0;
//  }
                int err = connect(sock_fd2,(struct sockaddr *)&info2,sizeof(info2));
                if(err==-1){
                    cerr << "Connection error\n";
                    return 0;
                }

//**************Encryption***************


                FILE *pri;
                RSA *privateRSA = nullptr;
                if((pri = fopen("payer_pri.pem","r")) == NULL) {
                    cout << "pri Error" << endl;
                    exit(-1);
                }
                // 初始化算法庫
                OpenSSL_add_all_algorithms();
                // 從 .pem 格式讀取公私鑰
                if((privateRSA = PEM_read_RSAPrivateKey(pri, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read pri error" << endl;
                }
                fclose(pri);
                int rsa_len = RSA_size(privateRSA); // 幫你算可以加密 block 大小，字數超過要分開加密
                
                const unsigned char * src = (const unsigned char *)money; //  測試的明文
                // 要開空間來存放加解密結果，型態要改成 unsigned char *

                unsigned char * enc = (unsigned char *)malloc(rsa_len);
                // 加密時因為 RSA_PKCS1_PADDING 的關係，加密空間要減 11，回傳小於零出錯
                if(RSA_private_encrypt(rsa_len-11, src, enc, privateRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "enc error" << endl;
                }
                cout << "enc: " << enc << endl;
                RSA_free(privateRSA);


//***************Encryption********************

                send(sock_fd2, enc, strlen((const char*)enc), 0);
                sleep(1);
                }//for user exist
            }
            else
                wait(&status);
  }
  else if(strcmp("Wait", command)==0)
        {
            int status, fd[2];
            pipe(fd);
            char enc_message[300];

            if(fork() == 0)
            {
                cout << "Please enter your port again:";
                int socket_desc2 , new_socket2 , c2, port_temp;
                cin >> port_temp;
                struct sockaddr_in server2 , client2;
                char *message2;
                
                //Create socket
                socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
                if (socket_desc2 == -1)
                {
                    printf("Could not create socket");
                }
                
                //Prepare the sockaddr_in structure
                server2.sin_family = PF_INET;
                server2.sin_addr.s_addr = inet_addr("127.0.0.1");
                server2.sin_port = htons( port_temp );
                
                //Bind
                if( bind(socket_desc2,(struct sockaddr *)&server2 , sizeof(server2)) < 0)
                {
                    puts("bind failed");
                    return 1;
                }
                puts("bind done");
                
                //Listen
                listen(socket_desc2 , 3);
                
                //Accept and incoming connection
                puts("Waiting for incoming connections...");
                c2 = sizeof(struct sockaddr_in);
                new_socket2 = accept(socket_desc2, (struct sockaddr *)&client2, (socklen_t*)&c2);
                if (new_socket2<0)
                {
                    perror("accept failed");
                    return 1;
                }
                
                puts("Connection accepted");


//**************Encryption***************
                FILE *payer, *payee;
                RSA *payerRSA = nullptr, *payeeRSA = nullptr;
                if((payer = fopen("payer_pub.pem","r")) == NULL) {
                    cout << "payer Error" << endl;
                    exit(-1);
                }
                if((payee = fopen("payee_pub.pem","r")) == NULL) {
                    cout << "payee Error" << endl;
                    exit(-1);
                }
                // 初始化算法庫
                OpenSSL_add_all_algorithms();
                // 從 .pem 格式讀取公私鑰
                if((payerRSA = PEM_read_RSA_PUBKEY(payer, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read payer error" << endl;
                }
                fclose(payer);
                if((payeeRSA = PEM_read_RSA_PUBKEY(payee, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read payee error" << endl;
                }
                fclose(payee);
                int rsa_len = RSA_size(payerRSA); // 幫你算可以加密 block 大小，字數超過要分開加密
                
                char recv_enc[1000];
                recv(new_socket2, recv_enc, strlen((const char*)recv_enc), 0);
                sleep(1);
                cout << "Received message: " << recv_enc << endl;

                const unsigned char * src = (const unsigned char *)recv_enc; //  測試的明文
                // 要開空間來存放加解密結果，型態要改成 unsigned char *

                unsigned char * dec = (unsigned char *)malloc(rsa_len); 
                // 加密時因為 RSA_PKCS1_PADDING 的關係，加密空間要減 11，回傳小於零出錯
                // if(RSA_public_decrypt(rsa_len, src, dec, payeeRSA, RSA_PKCS1_PADDING) < 0) {
                //     cout << "dec error" << endl;
                // }
                if(RSA_public_decrypt(rsa_len, dec, dec, payerRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "dec error" << endl;
                }

                 if(RSA_private_encrypt(rsa_len-11, src, dec, payeeRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "enc error" << endl;
                }
                // 加密後就會變成一堆奇怪字元
                // cout << "dec: " << dec << endl;
                // 因為是它的函式 new 出來的東東，需要用他的函式釋放記憶體
                cout << "enc: " << dec << endl;
                RSA_free(payerRSA);
                RSA_free(payeeRSA);
//----
                // FILE *pri;
                // RSA *privateRSA = nullptr;
                // if((pri = fopen("payee_pri.pem","r")) == NULL) {
                //     cout << "pri Error" << endl;
                //     exit(-1);
                // }
                // // 初始化算法庫
                // OpenSSL_add_all_algorithms();
                // // 從 .pem 格式讀取公私鑰
                // if((privateRSA = PEM_read_RSAPrivateKey(pri, NULL,NULL,NULL)) == NULL) { 
                //     cout << "Read pri error" << endl;
                // }
                // fclose(pri);
                // int rsa_len = RSA_size(privateRSA); // 幫你算可以加密 block 大小，字數超過要分開加密
                

                // char recv_enc[1000];
                // recv(new_socket2, recv_enc, strlen((const char*)recv_enc), 0);
                // sleep(1);
                // cout << "Received message: " << recv_enc << endl;

                // const unsigned char * src = (const unsigned char *)recv_enc; //  測試的明文
                // // 要開空間來存放加解密結果，型態要改成 unsigned char *
    
                // unsigned char * enc = (unsigned char *)malloc(rsa_len);
                // // 加密時因為 RSA_PKCS1_PADDING 的關係，加密空間要減 11，回傳小於零出錯
                // if(RSA_private_encrypt(rsa_len-11, src, enc, privateRSA, RSA_PKCS1_PADDING) < 0) {
                //     cout << "enc error" << endl;
                // }
                // // 加密後就會變成一堆奇怪字元
                // // 因為是它的函式 new 出來的東東，需要用他的函式釋放記憶體
                // RSA_free(privateRSA);
                // cout << endl << enc << endl;


//***************Encryption********************

                close(fd[0]);
                write(fd[1], dec, sizeof(dec));
                close(fd[1]);

                exit(0);
            }
            else
            {
                wait(&status);
                close(fd[1]);
                read(fd[0], enc_message, sizeof(enc_message));
                close(fd[0]);
            }
            cout << endl << "encoding message:" << enc_message << endl;
            send(socket_fd, enc_message, strlen((const char*)enc_message), 0);

        }


 //to quit
  else if(strcmp("Exit", command)==0){
   strcpy(output, "Exit\n");
   send(socket_fd, output, sizeof(output), 0);
   recv(socket_fd, Message_Received, sizeof(Message_Received), 0);
   cerr << Message_Received << endl;
   if(strstr(Message_Received, "Bye") != NULL){
      login = false;
   }
   return 0;
  }
  else{
    cerr << "Invalid Entry, please try again." << endl;
  }
 }
 return 0;
}