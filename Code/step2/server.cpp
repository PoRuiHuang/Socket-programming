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
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>

using namespace std;

void* connection(void*);
pthread_mutex_t mutex;
char IP_ad[100];


class Client{
  public:     
    Client(string n): name(n){}
    ~Client(){}
    const string& getName() const { return this->name; }
    const char*getIP() const { char* str=IP_ad; return str; }
    void setPort(string port){this->port = port;}
    string getPort() const { return port; }
    void on_line(){this->online = true;}
    void off_line(){this->online = false;} 
    bool is_online(){if(online == true) return true; 
                                      else return false;}
  private:
    string name;
    string port;
    int account_balance = 10000;
    bool online = false;
};
vector <Client> C_list;


bool myStr2Int(const string& str, int& num){
   num = 0;
   size_t i = 0;
   int sign = 1;
   if (str[0] == '-') { sign = -1; i = 1; }
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += int(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   num *= sign;
   return valid;
}

void reverse(char str[], int length) { 
    int start = 0; 
    int end = length -1; 
    while (start < end) 
    { 
        swap(*(str+start), *(str+end)); 
        start++; 
        end--; 
    } 
}
char* itoa(int num, char* str, int base){
    int i = 0; 
    bool isNegative = false; 
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
  
    // In standard itoa(), negative numbers are handled only with  
    // base 10. Otherwise numbers are considered unsigned. 
    if (num < 0 && base == 10) { 
        isNegative = true; 
        num = -num; 
    } 
  
    // Process individual digits 
    while (num != 0) { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
  
    // If number is negative, append '-' 
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0'; // Append string terminator 
  
    // Reverse the string 
    reverse(str, i); 
  
    return str; 
} 

int main(){
//create a socket file and make the connection
 int socket_fd = 0;
 int new_socket;



//socket creating
 socket_fd = socket(AF_INET, SOCK_STREAM, 0);
 if(socket_fd == -1){  //if error
  cerr << "Fail to create a socket.";
  return 0;
 }
 
//server  setting
 struct sockaddr_in server, client;
 bzero(&server, sizeof(server));
 server.sin_family = AF_INET;
 server.sin_addr.s_addr = inet_addr("127.0.0.1");
 server.sin_port = htons(2104);



if(bind(socket_fd, (struct sockaddr*) &server, sizeof(server)) == -1){  //if error
  cerr << "Fail to bind a socket.";
  return 0;
 }
cout << "waiting for connection..." << endl;
int _lis = listen(socket_fd, 5);
if(_lis == -1){  //if error
  cerr << "Fail to listen.";
  return 0;
 }
int c = sizeof(struct sockaddr_in);
while((new_socket = accept(socket_fd, (struct sockaddr*) &client,(socklen_t*)&c))){
    
    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char c_id[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, c_id, INET_ADDRSTRLEN );
    strcpy(IP_ad,c_id);
    cout << "Connection complete" << endl;
    pthread_t sniff;
    int* newsock;
    newsock = (int*)malloc(1);
    *newsock = new_socket;
    if(pthread_create(&sniff, nullptr, connection,(void*)newsock)==-1){
          cerr << "Fail to create thread.";
          return 0;
    }
    //pthread_join(sniff,NULL);
}

if(new_socket==-1){
  cerr << "Fail to accept.";
  return 0;
}

}

void* connection(void* socket_fd){
  
    int sock = *(int*)socket_fd;
    char rec_message[2048];
    char send_message[2048];

    memset(rec_message, '\0', sizeof(rec_message));
    memset(send_message, '\0', sizeof(send_message));
    strcpy(send_message,"connection success\n");
    write(sock, send_message, strlen(send_message));
    memset(send_message, '\0', sizeof(send_message));

    int message_size;
    Client *cur_client;
    while((message_size = recv(sock, rec_message, 2048, 0))>0){
        string s = (string) rec_message;

        cout << s;
        int mark = s.find('#');
        if(mark!= string::npos){
            string command = s.substr(0,mark);
            if(command =="REGISTER"){
                string n = s.substr(mark+1, s.size()-mark-2);
                bool named = false;
                pthread_mutex_lock(&mutex);
                for(size_t i = 0; i< C_list.size(); i ++){
                    if(C_list[i].getName() == n){
                        named = true;
                        break;
                    }
                }
                if(named ==false){
                    Client user(n);
                    C_list.push_back(user);
                    strcpy(send_message,"100 OK\n");
                    write(sock, send_message, strlen(send_message));
                    memset(send_message, '\0', sizeof(send_message));
                }
                else{
                    strcpy(send_message,"210 FAIL\n");
                    write(sock, send_message, strlen(send_message));
                    memset(send_message, '\0', sizeof(send_message));
                }
                pthread_mutex_unlock(&mutex);
            }
            else{
                pthread_mutex_lock(&mutex);
                bool in_L = false;
                string n = s.substr(mark+1, s.size()-mark-2);
                int _port;
                for(size_t i = 0; i< C_list.size(); i ++){
                    if(C_list[i].getName() == command && C_list[i].is_online()== false && myStr2Int(n, _port)==true){
                        in_L = true;
                        C_list[i].setPort(n);
                        cur_client = &C_list[i];
                        C_list[i].on_line();
                        int on_num = 0;
                        for(size_t i = 0; i< C_list.size(); i ++){
                           if(C_list[i].is_online()==true) on_num++;  
                        }

 
                        char ch_num[100];
                        strcpy(send_message,"10000\n");
                        strcat(send_message,"number of accounts online: ");
                        strcat(send_message, itoa(on_num, ch_num,10));
                        strcat(send_message, "\n");

                        for(size_t i = 0; i< C_list.size(); i ++){
                           if(C_list[i].is_online()==true){
                                strcat(send_message,C_list[i].getName().c_str());
                                strcat(send_message,"#");
                                strcat(send_message,C_list[i].getIP());
                                strcat(send_message,"#");
                                strcat(send_message,C_list[i].getPort().c_str());
                                strcat(send_message, "\n");
                           }
                        }
                        write(sock, send_message, strlen(send_message));
                        memset(send_message, '\0', sizeof(send_message));
                        break;
                    }
                }
                if(in_L == false){
                    strcpy(send_message,"220 AUTH_FAIL\n");
                    write(sock, send_message, strlen(send_message));
                    memset(send_message, '\0', sizeof(send_message));
                }
                pthread_mutex_unlock(&mutex);
            }
        }
        else{
            if(s == "List\n"){
                pthread_mutex_lock(&mutex);
                 int on_num = 0;
                        for(size_t i = 0; i< C_list.size(); i ++){
                           if(C_list[i].is_online()==true) on_num++;  
                        }

 
                        char ch_num[100];
                        strcpy(send_message,"10000\n");
                        strcat(send_message,"number of accounts online: ");
                        strcat(send_message, itoa(on_num, ch_num,10));
                        strcat(send_message, "\n");

                        for(size_t i = 0; i< C_list.size(); i ++){
                           if(C_list[i].is_online()==true){
                                strcat(send_message,C_list[i].getName().c_str());
                                strcat(send_message,"#");
                                strcat(send_message,C_list[i].getIP());
                                strcat(send_message,"#");
                                strcat(send_message,C_list[i].getPort().c_str());
                                strcat(send_message, "\n");
                           }
                        }
                        write(sock, send_message, strlen(send_message));
                        memset(send_message, '\0', sizeof(send_message));
                pthread_mutex_unlock(&mutex);
            }
            else if(s == "Exit\n"){
                cur_client->off_line();
                strcpy(send_message,"Bye\n");
                write(sock, send_message, strlen(send_message));
                memset(send_message, '\0', sizeof(send_message));
                pthread_exit(0);
            }
            else{
                strcpy(send_message,"220 AUTH_FAIL\n");
                write(sock, send_message, strlen(send_message));
                memset(send_message, '\0', sizeof(send_message));
            }  
        }

        memset(rec_message, '\0', sizeof(rec_message));
        memset(send_message, '\0', sizeof(send_message));
    }
 

}