#include "sqlite3.h"
#include "Logger/logger.hpp"
#include <iostream>
#include <fstream>
#include "products/products.hpp"

using std::to_string;
using std::vector;
using std::stoi;
using std::ofstream;
using std::ios;
using std::endl;
using std::mutex;
using std::getline;
using std::ifstream;

string ordersFileName = "data/orders.txt";

string join(string delim, vector<string> strings) {
    string result = "";
    for (int i = 0; i < strings.size() - 1; i++) {
        //std::cout << strings[i] << std::endl;
        result = result + strings[i];
        result = result + delim;
    }
    result = result + strings[strings.size() - 1];
    //std::cout << result;

    return result;
} 

class Operation {
public:
    char* response = "";
    void createUser(string username, string password, int initialBalance) {
        // Write insert query here.
        sqlite3 *db;
        char *zErrMsg = 0;
        string sql;
        sqlite3_open(databaseFileName.c_str(), &db);
        
        sql = "INSERT INTO Users (username, password, balance) VALUES ('" + username + "', '" + password + "', " + to_string(initialBalance) + ");";

        sqlite3_exec(db, sql.c_str(), NULL, NULL, & zErrMsg);

        if (zErrMsg) {
            // If User exists, this will be printed.
            response = zErrMsg;
        }

        sqlite3_close(db);
    }

    void login(string username, string password) {
        // Write select query here.
        sqlite3 *db;
        char *zErrMsg = 0;
        string sql;
        sqlite3_open(databaseFileName.c_str(), &db);
        
        sql = "SELECT password from Users WHERE username=\"" + username + "\";";
        
        sqlite3_stmt* stmt = NULL;

        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt,NULL);

        int rc = sqlite3_step(stmt);

        try {
            string passwordOfUsername = string((char*)sqlite3_column_text(stmt, 0));
        
            if (passwordOfUsername == password) {
                response = "1";
            } else {
                response = "0";
            }
        } catch (char* exception) {
            // This will be used when the username is not found.
            response = "0";
        }   

        sqlite3_finalize(stmt);

        sqlite3_close(db);
    }

    void changePassword(string username, string newPassword) {
        // Write update query here.
        sqlite3 *db;
        char *zErrMsg = 0;
        string sql;
        sqlite3_open(databaseFileName.c_str(), &db);
        
        sql = "UPDATE Users SET password = \"" + newPassword + "\" WHERE username = \"" + username + "\";";

        sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);

        // We do not need to worry about this because the client's username will always be correct when calling this.
        // This statement will throw errors when the username is not found.

        sqlite3_close(db);
    }

    void getProductDetails() {
        // Write select query here.
        sqlite3 *db;
        char *zErrMsg = 0;
        string sql;
        sqlite3_open(databaseFileName.c_str(), &db);
        
        vector<string> productDetails;

        // Get smartphone details from smartphone.
        string sqlSmartphone = "SELECT productName,price,numberOfCameras,processor,ram,rom,countryOfOrigin FROM SmartphoneDetails;";
        string sqlLaptop = "SELECT productName,countryOfOrigin,price,ram,rom,hasTouchScreen FROM LaptopDetails;";
        
        sqlite3_stmt* stmt = NULL;

        sqlite3_prepare_v2(db, sqlSmartphone.c_str(), -1, &stmt,NULL);

        int ret_code;

        while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW){
            //std::cout << "going inside" << std::endl;
            string productName = string((char*)sqlite3_column_text(stmt, 0));
            int price = sqlite3_column_int(stmt, 1);
            int numberOfCameras = sqlite3_column_int(stmt, 2);
            string processor = string((char*)sqlite3_column_text(stmt, 3));
            int ram = sqlite3_column_int(stmt, 4);
            int rom = sqlite3_column_int(stmt, 5);
            string countryOfOrigin = string((char*)sqlite3_column_text(stmt, 6));

            Smartphone* s = new Smartphone(productName, countryOfOrigin, price, numberOfCameras, processor, ram, rom);
            //std::cout << s->to_str() << std::endl;
            productDetails.push_back(s->to_str());
        }

        sqlite3_finalize(stmt);

        stmt = NULL;

        sqlite3_prepare_v2(db, sqlLaptop.c_str(), -1, &stmt,NULL);

        ret_code = 0;

        while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
            //std::cout << "going inside" << std::endl;
            string productName = string((char*)sqlite3_column_text(stmt, 0));
            string countryOfOrigin = string((char*)sqlite3_column_text(stmt, 1));
            int price = sqlite3_column_int(stmt, 2);
            int ram = sqlite3_column_int(stmt, 3);
            int rom = sqlite3_column_int(stmt, 4);
            int hasTouchScreen = sqlite3_column_int(stmt, 5);

            Laptop* l = new Laptop(productName, countryOfOrigin, price, ram, rom, hasTouchScreen);
            productDetails.push_back(l->to_str());
        }

        string ans = join("\n", productDetails);

        sqlite3_finalize(stmt);

        this -> response = (char*)ans.c_str();

        sqlite3_close(db);

        //std::cout << "Method ends here.\n";
    }
    
    void buy(string type, string username, string productName, int qauntity) {
        // Write select query here.
        sqlite3 *db;
        char *zErrMsg = 0;
        string sql;
        sqlite3_open(databaseFileName.c_str(), &db);
        
        sql = "SELECT quantity FROM Stock WHERE productName = '" + productName + "';";

        sqlite3_stmt *stmt = NULL;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        if (rc != SQLITE_OK) {
            Logger::Error("Failed to prepare statement");
            return;
        }

        rc = sqlite3_step(stmt);

        int currentStock = sqlite3_column_int(stmt, 0);

        if (currentStock < qauntity) {
            response = "Insufficient stock";
            return;
        }

        rc = sqlite3_finalize(stmt);

        // Check balance of user now.

        sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

        stmt = NULL;

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        if (rc != SQLITE_OK) {
            Logger::Error("Failed to prepare statement");
            return;
        }

        rc = sqlite3_step(stmt);

        if (rc == SQLITE_DONE) {
            response = "Insufficient balance";
            return;
        }

        int balance = sqlite3_column_int(stmt, 0);

        rc = sqlite3_finalize(stmt);

        // Check for the price of the product from laptopDetails or smartphoneDetails 
        // and deduct the price from balance.

        //std::cout << "Type is " << type << std::endl;

        if (type == "smartphone") {
            sql = "SELECT price FROM SmartphoneDetails WHERE productName = '" + productName + "';";
        } else if (type == "laptop") {
            sql = "SELECT price FROM LaptopDetails WHERE productName = '" + productName + "';";
        }

        stmt = NULL;
        // prepare the statement
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        rc = sqlite3_step(stmt);

        if (rc == SQLITE_DONE) {
            response = "Product not available.";
            return;
        }

        int price = sqlite3_column_int(stmt, 0);

        rc = sqlite3_finalize(stmt);

        //std::cout << "Works till here.2\n";

        sql = "SELECT price FROM SmartphoneDetails WHERE productName = '" + productName + "';";

        if (balance >= qauntity * price) {
            balance -= qauntity * price;
            sql = "UPDATE Users SET balance = " + to_string(balance) + " WHERE username = '" + username + "';";
            rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK) {
                Logger::Error("Failed to update balance");
                return;
            }
            sql = "UPDATE Stock SET quantity = quantity - " + to_string(qauntity) + " WHERE productName = '" + productName + "';";
            rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK) {
                Logger::Error("Failed to update stock");
                return;
            }
            response = "Success";

            // We need to append this order to data/orders.txt
            vector<string> partsToAppend = {username, productName, to_string(qauntity), to_string(price)};
            string order = join(",", partsToAppend);
            //std::cout << "Order is " << order << std::endl;
            //std::cout << username << " " << productName << " " << qauntity << " " << price << std::endl;

            mutex mt;
        
            mt.lock();
            // Making our write operation thread safe.
            // Only one thread should be able to access to the file at a time.
            ofstream myfile;
            myfile.open ("data/orders.txt", ios::app);
            myfile << order << endl;
            myfile.close();
            mt.unlock();
            
        } else {
            response = "Insufficient balance";
        }

        sqlite3_close(db);
    }
    
    void addMoney(string username, int amount) {
        // Update User balance and add amount.
        sqlite3 *db;

        char *zErrMsg = 0;

        string sql;

        sqlite3_open(databaseFileName.c_str(), &db);

        sql = "UPDATE Users SET balance = balance + " + to_string(amount) + " WHERE username = '" + username + "';";

        int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);
        if (zErrMsg) {
            char* response = zErrMsg;
            return;
        }
        char* response = "Added money";
    }

    void getOrders(string username) {
        // A user can access his/her orders.
        vector<string> orders;

        mutex mt;
        
        mt.lock();
        // Making our write operation thread safe.
        // Only one thread should be able to access to the file at a time.
        
        ifstream myfile;
        myfile.open (ordersFileName);

        string line;

        while (getline(myfile, line)) {
            if (line.find(username) == 0) {
                orders.push_back(line);
            }
        }

        mt.unlock();

        string ans = join("\n", orders);
        //std::cout << ans;
        response = (char*)malloc(sizeof(char) * (ans.length() + 1));
        strcpy(response, ans.c_str());
        // I am doing this because it throws garbage value otherwise.
        // c_str returns the char* used by this local string object.
        // This does not send a copy of the string.
        // So, it gets freed after the completion of this function.
        //std::cout << response << endl;
    }

    void getBalance(string username) {
        sqlite3* db;
        char* zErrMsg = 0;
        string sql;

        sqlite3_open(databaseFileName.c_str(), &db);

        sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

        sqlite3_stmt *stmt = NULL;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        if (rc != SQLITE_OK) {
            Logger::Error("Failed to prepare statement");
            return;
        }

        rc = sqlite3_step(stmt);

        if (rc == SQLITE_DONE) {
            response = "User not found";
            return;
        }

        int balance = sqlite3_column_int(stmt, 0);

        rc = sqlite3_finalize(stmt);

        string balanceStr = to_string(balance);
        response = (char*) malloc(sizeof(char) * (balanceStr.length() + 1));
        strcpy(response, balanceStr.c_str());
        // I am doing this because it throws garbage value otherwise.
        // c_str returns the char* used by this local string object.
        // This does not send a copy of the string.
        // So, it gets freed after the completion of this function.
        //std::cout << response << endl;

        sqlite3_close(db);
    }
};