#include <iostream>
#include <algorithm>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <openssl/sha.h>
//! For SHA512 function.
#include <vector>
#include <cstdlib>
#include "../include/logger.hpp"
#include "../include/splitAndJoin.hpp"

using std::string;
using std::cout;
using std::cin;
using std::transform;
using std::to_string;
using std::endl;
using std::vector;

#define PORT 54000

string seperator = ",";

/**
 * @brief Gets the sha512 hash of the given string.
 * 
 * @param toHash 
 * @return string 
 */
string sha512(string toHash) {
    unsigned char* hash = (unsigned char*) malloc(SHA512_DIGEST_LENGTH);
    SHA512((const unsigned char*)toHash.c_str(), toHash.length(), hash);
    return string((char*) hash);
}

/**
 * @brief Sends the request to the server and gets back the response.
 * 
 * @param sock 
 * @param toSendParts 
 * @return string 
 */
string handleRequest(int sock, vector<string> toSendParts) {
    string toSend = join(seperator, toSendParts);
    int sendRes = send(sock, toSend.c_str(), toSend.size(), 0);

    if (sendRes == -1) {
        Logger::Error("Cannot send the data to the server");
        return "";
    }

    char buf[SOMAXCONN];
    //! Making this buffer dynamic so it does not get removed from the memory.
    
    int valRecv = recv(sock, buf, SOMAXCONN, 0);
    buf[valRecv] = '\0';    

    return string(buf);
}

/**
 * @brief The main function for the client.
 * The main client application.
 * 
 * @return int 
 */
int main() {
    Logger::EnableFileOutput();

    //!	Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    char buf[SOMAXCONN];
    string userInput;

    string option;

    string username = "", password = "";

    if (sock == -1) {
        Logger::Error("Socket not connected.");
    }

    //!	Create a hint structure for the server we're connecting with
    string ipAddress = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    //!	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        Logger::Error("Connection issue.");
    }

    while (true) {
        cout << "Options:\n";
        cout << "\tEnter quit to quit.\n";
        cout << "\tEnter clear to clear screen\n";
        cout << "\tEnter login to login\n";
        cout << "\tEnter sign_up to sign up\n";
        cout << "Enter option: ";
        cin >> option;
        
        transform(option.begin(), option.end(), option.begin(), ::tolower);
        //! We convert string to lower case.

        if (option == "quit") {
            return 0;
            //! q is entered means the person wants to quit.
        } else if (option == "clear") {
            #if defined(_WIN32) || defined(__CYGWIN__) 
                system("cls");
            #else 
                system("clear");
            #endif
        } else if (option == "login") {
            cout << "Enter username: ";
            cin >> username;

            cout << "Enter password: ";
            cin >> password;

            password = sha512(password);
            vector<string> toSendParts = {option, username, password};
            //! We are sending password's hash has we cannot send the passwords in plain text.

            string response = handleRequest(sock, toSendParts);

            if (response == "1") {
                cout << "Login successful\n";
                break;
            } else if (response == "0") {
                cout << "Login Failed, try again\n";
            } else {
                Logger::Critical("Server code Login function has issues");
                return EXIT_FAILURE;
            }
        } else if (option == "sign_up") {
            int initialBalance;

            cout << "Enter username: ";
            cin >> username;

            cout << "Enter password: ";
            cin >> password;
            password = sha512(password);

            cout << "Enter initial Balance: ";
            cin >> initialBalance;

            vector<string> toSendParts = {option, username, password, to_string(initialBalance)};
            
            string response = handleRequest(sock, toSendParts);

            if (response == "0") {
                cout << "User already exists\n";
            } else {
                cout << "Sign up sucessful\n";
                break;
            }
        } else {
            cout << "You entered wrong option.\n";
        }
    } 

    //! So, we have logged in has username.
    while (true) {
        cout << "Options:\n";
        cout << "\tEnter clear to clear screen\n";
        cout << "\tEnter change_password to change the password\n";
        cout << "\tEnter get_product_details to get the product Details\n";
        cout << "\tEnter get_product_names to get the names of the products\n";
        cout << "\tEnter buy to buy a product\n";
        cout << "\tEnter add_money to add money to your account\n";
        cout << "\tEnter order_history to get history of your orders\n";
        cout << "\tEnter get_balance to get balance\n";
        
        cout << "Enter the option: ";
        cin >> option;
        transform(option.begin(), option.end(), option.begin(), ::tolower);

        if (option == "quit") {
            return 0;
        } else if (option == "clear") {
            #if defined(_WIN32) || defined(__CYGWIN__) 
                system("cls");
            #else 
                system("clear");
            #endif
        } else if (option == "change_password") {
            string oldPassword, newPassword;
            cout << "Enter old password: ";
            cin >> oldPassword;
            cout << "Enter new password: ";
            cin >> newPassword;

            if (sha512(oldPassword) == password) {
                newPassword = sha512(newPassword);
                vector<string> toSendParts = {option, username, newPassword};
                
                handleRequest(sock, toSendParts);
                
                //! We do not need to worry about the response here because only error possible is of username.
                //! We have to receive an empty string because server sends it.
                cout << "Your password is changed.\n";
                password = newPassword;
            } else {
                cout << "You entered wrong old password.\n";
            } 
        } else if (option == "get_product_details") {
            string type;

            cout << "Enter product name to get details of a product\n";
            cout << "Enter smartphone to get details of smartphones and laptop to get details for laptops\n";
            cout << "Enter all for details of all products\n";

            cout << "Enter option: ";
            cin >> type;

            transform(type.begin(), type.end(), type.begin(), ::tolower);

            vector<string> toSendParts = {option, type};
            string response = handleRequest(sock, toSendParts);

            cout << response << endl;
        } else if (option == "get_product_names") {
            string type;
            
            cout << "Enter product name to get details of a product\n";
            cout << "Enter smartphone to get details of smartphones and laptop to get details for laptops\n";
            cout << "Enter all for details of all products\n";

            cout << "Enter option: ";
            cin >> type;

            transform(type.begin(), type.end(), type.begin(), ::tolower);

            vector<string> toSendParts = {option, type};
            string response = handleRequest(sock, toSendParts);

            cout << response << endl;
        } else if (option == "buy") {
            string productName;
            int quantity;

            cout << "Enter the product name: ";
            cin >> productName;
            cout << "Enter the quantity: ";
            cin >> quantity;
        
            vector<string> toSendParts = {option, productName, to_string(quantity), username};
            
            string response = handleRequest(sock, toSendParts);

            cout << response << endl;
        } else if (option == "add_money") {
            int balance;
            cout << "Enter the amount you want to deposit: ";
            cin >> balance;

            vector<string> toSendParts = {option, username, to_string(balance)};
            
            string response = handleRequest(sock, toSendParts);
            
            cout << response << endl;
        } else if (option == "order_history") {
            vector<string> toSendParts = {option, username};
            
            string response = handleRequest(sock, toSendParts);

            cout << response << endl;
        } else if (option == "get_balance") {
            vector<string> toSendParts = {option, username};
            
            string response = handleRequest(sock, toSendParts);

            cout << response << endl;
        } else {
            cout << "You entered wrong option\n";
        }

    } 

    Logger::CloseFileOutput();
    return 0;
}