#include "sqlite3.h"
#include "../Logger/logger.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include "../products/products.hpp"

using std::to_string;
using std::vector;
using std::stoi;
using std::ofstream;
using std::ios;
using std::endl;
using std::mutex;
using std::getline;
using std::ifstream;

string seperator = ",";
string ordersFileName = "data/orders.txt";
mutex ordersFileMutex;

/**
 * @brief Joins all the strings with delim between them.
 * 
 * @param delim 
 * @param strings 
 * @return string 
 */
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

/**
 * @brief An easier way to get int column.
 * 
 * @param stmt 
 * @param colName 
 * @return int 
 */
int getSQLInt(sqlite3_stmt* stmt, int colName) {
    return sqlite3_column_int(stmt, colName);
}

/**
 * @brief An easier way to get string column.
 * 
 * @param stmt 
 * @param colName 
 * @return string 
 */
string getSQLText(sqlite3_stmt* stmt, int colName) {
    return string((char*)sqlite3_column_text(stmt, colName));
}

/**
 * @brief The operation class contains all the operations to be performed on the database.
 * 
 */
class Operation {
    sqlite3* db;
    bool isSharedConnection;
    char* response = NULL;
public:
    /**
     * @brief Construct a new Operation object
     * Creates it's own connection to the database.
     * To be used when SQLITE is in MULTI-THREADED mode.
     */
    Operation() {
        sqlite3_open(databaseFileName.c_str(), &db);
        isSharedConnection = false;
    }

    /**
     * @brief Construct a new Operation object
     * Gets the shared database object.
     * To be used when SQLITE is in SERIALIZED mode.
     * @param db 
     */
    Operation(sqlite3* db) {
        this->db = db;
        isSharedConnection = true;
    }

    /**
     * @brief Get the Response object
     * 
     * @return char* 
     */
    char* getResponse() {
        return response;
    }

    /**
     * @brief Create a User in the in the database.
     * 
     * @param username 
     * @param password 
     * @param initialBalance 
     */
    void createUser(string username, string password, int initialBalance) {
        // Write insert query here.
        char *zErrMsg = 0;
        string sql = "INSERT INTO Users (username, password, balance) VALUES ('" + username + "', '" + password + "', " + to_string(initialBalance) + ");";

        sqlite3_exec(db, sql.c_str(), NULL, NULL, & zErrMsg);

        if (zErrMsg) {
            // If User exists, this will be printed.
            response = (char*)"0";
        } else {
            response = (char*)"1";
        }
    }

    /**
     * @brief Checks if the login credintials are correct.
     * 
     * @param username 
     * @param password 
     */
    void login(string username, string password) {
        // Write select query here.
        string sql = "SELECT password from Users WHERE username='" + username + "';";
        
        sqlite3_stmt* stmt = NULL;

        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        int rc = sqlite3_step(stmt);

        if (rc == SQLITE_DONE) {
            response = (char*)"0";
            return;
        }

        string passwordOfUsername = string((char*)sqlite3_column_blob(stmt, 0));
        
        if (passwordOfUsername == password) {
            response = (char*)"1";
        } else {
            response = (char*)"0";
        }

        sqlite3_finalize(stmt);

    }

    /**
     * @brief Changes password in the database.
     * 
     * @param username 
     * @param newPassword 
     */
    void changePassword(string username, string newPassword) {
        // Write update query here.
        char *zErrMsg = 0;
        string sql = "UPDATE Users SET password = '" + newPassword + "' WHERE username = '" + username + "';";

        sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);

        response = (char*)"Password changed successfully.";
        // We do not need to worry about this because the client's username will always be correct when calling this.
        // This statement will throw errors when the username is not found.
    }

    /**
     * @brief Get the Product Details from the database.
     * 
     * @param userWants 
     */
    void getProductDetails(string userWants) {
        //printf("%s %d\n", userWants.c_str(), userWants.length());
        
        string sql;
        sqlite3_stmt* stmt = NULL;
        int rc, type, price, stock, ram, rom, hasTouchScreen, numberOfCameras;
        string productName, countryOfOrigin, processor;

        // We use stock > 0 because we want to show a product to the user only when stock is there for the product.
        if (userWants == "all") {
            sql = "SELECT * FROM ProductDetails WHERE stock > 0;";
        } else if (userWants == "smartphone") {
            sql = "SELECT * FROM ProductDetails WHERE type = " + to_string(Product::smartphone) + " AND stock > 0;";
        } else if (userWants == "laptop") {
            sql = "SELECT * FROM ProductDetails WHERE type = " + to_string(Product::laptop) + " AND stock > 0;";
        } else {
            sql = "SELECT * FROM ProductDetails WHERE productName = '" + userWants + "' COLLATE NOCASE AND stock > 0;";
        }
        // Returns table details in format: (productName, countryOfOrigin, price, ram, rom, hasTouchScreen, numberOfCameras, stock, processor, type).

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        vector<string> productDetails;

        int rowCount = 0;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            rowCount++;

            stock = getSQLInt(stmt, 7);
            productName = getSQLText(stmt, 0);
            countryOfOrigin = getSQLText(stmt, 1);
            price = getSQLInt(stmt, 2);
            type = getSQLInt(stmt, 9);

            if (type == Product::smartphone) {
                ram = getSQLInt(stmt, 3);
                rom = getSQLInt(stmt, 4);
                numberOfCameras = getSQLInt(stmt, 6);
                processor = getSQLText(stmt, 8);

                productDetails.push_back(Smartphone(productName, countryOfOrigin, price, stock, numberOfCameras, processor, ram, rom).toStr());
            } else if (type == Product::laptop) {
                ram = getSQLInt(stmt, 3);
                rom = getSQLInt(stmt, 4);
                hasTouchScreen = getSQLInt(stmt, 5);

                productDetails.push_back(Laptop(productName, countryOfOrigin, price, stock, ram, rom, hasTouchScreen).toStr());
            } else {
                Logger::Critical("Product of unknown type in the database. Corruption of data possible.");
            }
        }

        sqlite3_finalize(stmt);

        if (rowCount == 0) {
            response = (char*) "Product Not found";
            return;
        }

        string toSend = join("\n", productDetails);
        response = new char[toSend.size()];
        strcpy(response, toSend.c_str());
    }
    
    /**
     * @brief Get the Product Names from the database.
     * 
     * @param type 
     */
    void getProductNames(string type) {
        string sql, productName;
        vector<string> products;
        
        if (type == "all") {
            sql = "SELECT productName FROM ProductDetail WHERE stock > 0;";
        } else if (type == "smartphone") {
            sql = "SELECT productName FROM ProductDetails WHERE type = " + to_string(Product::smartphone) + " AND stock > 0;";
        } else if (type == "laptop") {
            sql = "SELECT productName FROM ProductDetails WHERE type = " + to_string(Product::laptop) + " AND stock > 0;";
        } else {
            response = (char*) "Wrong type entered";
            return;
        }

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            productName = getSQLText(stmt, 0);
            products.push_back(productName);
        }

        sqlite3_finalize(stmt);

        string toSend = join("\n", products);
        response = new char[toSend.size()];
        strcpy(response, toSend.c_str());        
    }

    /**
     * @brief Buys a product from the database.
     * 
     * @param username 
     * @param productName 
     * @param qauntity 
     */
    void buy(string username, string productName, int qauntity) {
        // Write select query here.
        char *zErrMsg = 0;
        int currentStock, price, rc;
        string sql;

        sqlite3_stmt* stmt;

        try {    
            string sql = "SELECT stock,price FROM ProductDetails WHERE productName = '" + productName + "';";
        
            sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

            rc = sqlite3_step(stmt);

            if (rc == SQLITE_DONE) {
                throw (char*) "Product Not found";
            }

            currentStock = getSQLInt(stmt, 0);
            price = getSQLInt(stmt, 1);

            if (currentStock < qauntity) {
                throw (char*) "Not enough stock";
            }
            sqlite3_finalize(stmt);
        } catch (char* error) {
            response = error;
            sqlite3_finalize(stmt);
        }

        // Check balance of user now.
        sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        rc = sqlite3_step(stmt);

        int balance = getSQLInt(stmt, 0);

        rc = sqlite3_finalize(stmt);

        // Check for the price of the product from laptopDetails or smartphoneDetails 
        // and deduct the price from balance.

        if (balance >= qauntity * price) {
            balance -= qauntity * price;
            sql = "UPDATE Users SET balance = " + to_string(balance) + " WHERE username = '" + username + "';";
            rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);

            sql = "UPDATE ProductDetails SET stock = stock - " + to_string(qauntity) + " WHERE productName = '" + productName + "';";
            rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);
            
            response = (char*) "Success";

            // We need to append this order to data/orders.txt
            vector<string> partsToAppend = {username, productName, to_string(qauntity), to_string(price)};
            string order = join(seperator, partsToAppend);
            //std::cout << "Order is " << order << std::endl;
            //std::cout << username << " " << productName << " " << qauntity << " " << price << std::endl;
        
            ordersFileMutex.lock();
            // Making our write operation thread safe.
            // Only one thread should be able to access to the file at a time.
            ofstream myfile;
            myfile.open ("data/orders.txt", ios::app);
            myfile << order << endl;
            myfile.close();
            ordersFileMutex.unlock();
        } else {
            response = (char*)"Insufficient balance";
        }
    }
    
    /**
     * @brief Adds money to user's account.
     * 
     * @param username 
     * @param amount 
     */
    void addMoney(string username, int amount) {
        // Update User balance and add amount.
        char *zErrMsg = NULL;
        
        string sql = "UPDATE Users SET balance = balance + " + to_string(amount) + " WHERE username = '" + username + "';";

        int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);
        
        if (zErrMsg) {
            response = zErrMsg;
            return;
        }
        response = (char*)"Added money";
    }

    /**
     * @brief Get the Orders of an user.
     * 
     * @param username 
     */
    void getOrders(string username) {
        // A user can access his/her orders.
        vector<string> orders;
        orders.push_back("Username,Product Name,Quantity,Price");

        ordersFileMutex.lock();
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

        ordersFileMutex.unlock();

        string ans = join("\n", orders);
        //std::cout << ans;
        response = new char[ans.length() + 1];
        strcpy(response, ans.c_str());
        // I am doing this because it throws garbage value otherwise.
        // c_str returns the char* used by this local string object.
        // This does not send a copy of the string.
        // So, it gets freed after the completion of this function.
        //std::cout << response << endl;
    }
    
    /**
     * @brief Get the account balance of an user.
     * 
     * @param username 
     */
    void getBalance(string username) {
        char* zErrMsg = 0;
        string sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

        sqlite3_stmt *stmt = NULL;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        rc = sqlite3_step(stmt);

        int balance = getSQLInt(stmt, 0);

        rc = sqlite3_finalize(stmt);

        string balanceStr = to_string(balance);
        response = new char[balanceStr.length() + 1];
        strcpy(response, balanceStr.c_str());
        // I am doing this because it throws garbage value otherwise.
        // c_str returns the char* used by this local string object.
        // This does not send a copy of the string.
        // So, it gets freed after the completion of this function.
    }

    /**
     * @brief Destroy the Operation object
     * Closes the database connection object if it openned it.
     * Does not close the connection object incase it was a shared one.
     */
    ~Operation() {
        if (!isSharedConnection) {
            sqlite3_close(db);
        }
    }
};