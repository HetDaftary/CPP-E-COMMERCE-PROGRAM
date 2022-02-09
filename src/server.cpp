#include "../include/operation.hpp"
//! #include <stdio.h> 
#include <string.h>   //!strlen 
//! #include <stdlib.h> 
//! #include <errno.h> 
#include <unistd.h>   //!close 
#include <arpa/inet.h>    //!close 
//! #include <sys/types.h> 
#include <sys/socket.h> 
//! #include <netinet/in.h> 
//! #include <sys/time.h> //!FD_SET, FD_ISSET, FD_ZERO macros 
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sqlite3.h>
#include "../include/logger.hpp"
#include "../include/splitAndJoin.hpp"

#define PORT 54000
#define PENDING_CONNECTIONS 10
#define THREAD_POOL_SIZE 10

using std::thread;
using std::condition_variable;
using std::queue;
using std::unique_lock;
using std::vector;

thread threadPool[THREAD_POOL_SIZE];
queue<int> connectionQueue;
mutex queueMutex;
condition_variable queueCondition;
string seperator = ",";

/**
 * @brief Tells us the thing to be send to user for the given request.
 * 
 * @param buffer: The buffer to be sent to user.
 * @param db: The database connection.
 * @return char* 
 */
char* solveRequest(char* buffer, sqlite3* db) {
    vector<string> request = split(seperator, string(buffer));

    Operation newOp(db);
                    
    if (request[0] == "login") {
        newOp.login(request[1], request[2]);
    } else if (request[0] == "sign_up") {
        newOp.createUser(request[1], request[2], stoi(request[3]));
    } else if (request[0] == "change_password") {
        newOp.changePassword(request[1], request[2]);
    } else if (request[0] == "get_product_details") {
        newOp.getProductDetails(request[1]);
    } else if (request[0] == "buy") {                        
        newOp.buy(request[3], request[1], stoi(request[2]));
    } else if (request[0] == "add_money") {
        newOp.addMoney(request[1], stoi(request[2]));
    } else if (request[0] == "order_history") {
        newOp.getOrders(request[1]);
    } else if (request[0] == "get_balance") {
        newOp.getBalance(request[1]);
    } else if (request[0] == "get_product_names") {
        newOp.getProductNames(request[1]);
    }

    return newOp.getResponse();
}

/**
 * @brief The stuff we have to do with a connection from a client.
 * 
 * @param socket: The client socket value.
 * @param db: The database connection object.
 */
void handleConnection(int socket, sqlite3* db) {
    char buffer[SOMAXCONN];
    int valread;
    while ((valread = read(socket, buffer, SOMAXCONN)) > 0) {
        buffer[valread] = '\0';
        char* response = solveRequest(buffer, db);
        write(socket, response, strlen(response));
    }
    close(socket);
}

/**
 * @brief The thread function for the threads of the thread pool.
 * They check for anywork and if found, they get it from the connection queue.
 * 
 * @param db: The database connection.
 */
void threadFunction(sqlite3* db) {
    int socket;
    while (true) {
        {
            unique_lock<mutex> lock(queueMutex);
            if (connectionQueue.empty()) {
                queueCondition.wait(lock, []{return !connectionQueue.empty();});
            }
            socket = connectionQueue.front();
            connectionQueue.pop();
        }
        handleConnection(socket, db);
    }
}

/**
 * @brief The main function for the server.
 * It starts the server and creates the thread pool.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[]) {
    printf("Server started\n");
    printf("Do not close this terminal window untill you want to close the server\n");
    printf("To close, press CTRL+C and then close the terminal window\n");

    sqlite3_config(SQLITE_CONFIG_SERIALIZED);
    sqlite3_config(SQLITE_OPEN_FULLMUTEX);

    sqlite3* db; 
    sqlite3_open(Operation::databaseFileName.c_str(), &db);

    Logger::EnableFileOutput();
    
    int opt = 1;
    int master_socket, addrlen, new_socket, client_socket[PENDING_CONNECTIONS], max_clients = PENDING_CONNECTIONS, activity, i, valread, sd;
    int max_sd;

    master_socket = socket(AF_INET, SOCK_STREAM, 0);

    //!set master socket to allow multiple connections , 
    //!this is just a good habit, it will work without this 
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {  
        Logger::Error("setsockopt");  
        exit(EXIT_FAILURE);  
    }  

    struct sockaddr_in address;
    
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  

    //!bind the socket to localhost port 54000 
    bind(master_socket, (struct sockaddr *)&address, sizeof(address));
         
    //!trying to specify maximum of pending connections for the master socket 
    listen(master_socket, PENDING_CONNECTIONS);

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        threadPool[i] = thread(threadFunction, db);
    }

    while (true) {
        addrlen = sizeof(address);
        new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        queueMutex.lock();
        connectionQueue.push(new_socket);
        queueCondition.notify_one();
        queueMutex.unlock();
    }

    sqlite3_close(db);
    return 0;
}

/*
int main(int argc , char *argv[]) {
    Logger::EnableFileOutput();  

    int opt = true;  
    int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
         
    char buffer[SOMAXCONN + 1];  //!data buffer of 1K 
         
    //!set of socket descriptors 
    fd_set readfds;  
     
    //!initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++) {  
        client_socket[i] = 0;  
    }  
         
    //!create a master socket 
    if((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {  
        Logger::Error("socket failed");  
        exit(EXIT_FAILURE);  
    }  
     
    //!set master socket to allow multiple connections , 
    //!this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {  
        Logger::Error("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
     
    //!type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
         
    //!bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {  
        Logger::Error("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
         
    //!try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, PENDING_CONNECTIONS) < 0) {  
        Logger::Error("listen");  
        exit(EXIT_FAILURE);  
    }  
         
    //!accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
         
    while(true) {  
        //!clear the socket set 
        FD_ZERO(&readfds);  
     
        //!add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
             
        //!add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++) {  
            //!socket descriptor 
            sd = client_socket[i];  
                 
            //!if valid socket descriptor then add to read list 
            if(sd > 0) {
                FD_SET( sd , &readfds);  
            }

            //!highest file descriptor number, need it for the select function 
            if(sd > max_sd) {  
                max_sd = sd; 
            } 
        }  
     
        //!wait for an activity on one of the sockets , timeout is NULL , 
        //!so wait indefinitely 
        activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR)) {  
            printf("select error");  
        }  
             
        //!If something happened on the master socket , 
        //!then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds)) {  
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {  
                Logger::Error("accept");  
                exit(EXIT_FAILURE);  
            }  
             
            //!inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                
            //!add new socket to array of sockets 
            for (i = 0; i < max_clients; i++) {  
                //!if position is empty 
                if(client_socket[i] == 0) {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                         
                    break;  
                }  
            }  
        }  
             
        //!else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++) {  
            sd = client_socket[i];  

            //! This function is used to check if socket is in set.   
            if (FD_ISSET( sd , &readfds)) {  
                //!Check if it was for closing , and also read the 
                //!incoming message 
                memset(buffer, 0, SOMAXCONN);
                if ((valread = read( sd , buffer, SOMAXCONN)) == 0) {  
                    //!Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                         
                    //!Close the socket and mark as 0 in list for reuse 
                    close(sd);  
                    client_socket[i] = 0;  
                }  
                     
                else {  
                    //!set the string terminating NULL byte on the end 
                    //!of the data read 
                    buffer[valread] = '\0';

                    char* response = solveRequest(buffer);
                    
                    int sendRes = send(client_socket[i], response, SOMAXCONN, 0);

                    if (sendRes == -1) {
                        Logger::Error("cannot send the data");
                    }
                }  
            }  
        }  
    }  
    
    Logger::CloseFileOutput();
    return 0;  
}  
*/
