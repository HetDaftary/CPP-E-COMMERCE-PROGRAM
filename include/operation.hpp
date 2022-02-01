#ifndef _OPERATION_HPP
#define _OPERATION_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <mutex>
#include <sqlite3.h>
#include "../include/splitAndJoin.hpp"

using std::to_string;
using std::vector;
using std::stoi;
using std::ofstream;
using std::ios;
using std::endl;
using std::mutex;
using std::getline;
using std::ifstream;
using std::string;

/**
 * @brief The operation class contains all the operations to be performed on the database.
 * 
 */
class Operation {
private:
    sqlite3* db;
    bool isSharedConnection;
    char* response = NULL;
    
    /**
     * @brief The return type of getDataFromSQL.
     * This is used to store the data from the SQL query.
     */
    typedef struct {
        vector<vector<char*>> dataStr;
        vector<vector<int>> dataInt;
    } SQLData;

    /**
     * @brief Will mainly be useful for SELECT queries.
     * Gets x rows with m string columns and n int columns.
     * NOTE: User needs to write query in a way that first m string columns are recevied and then n int columns.
     * 
     * @param query 
     * @param numberOfColumns 
     * @return SQLData 
     */
    SQLData getDataFromSQL(string query, int numberOfStrColumns, int numberOfIntColumns);

    /**
     * @brief To update database.
     * Mainly for Insert and Update queries.
     * 
     * @param query 
     * @return char* errorMsg.
     */
    char* putDataInDatabase(string query);
public:
    static const string databaseFileName;
    /**
     * @brief Construct a new Operation object
     * Creates it's own connection to the database.
     * To be used when SQLITE is in MULTI-THREADED mode.
     */
    Operation();

    /**
     * @brief Construct a new Operation object
     * Gets the shared database object.
     * To be used when SQLITE is in SERIALIZED mode.
     * @param db 
     */
    Operation(sqlite3* db);

    /**
     * @brief Get the Response object
     * 
     * @return char* 
     */
    char* getResponse();

    /**
     * @brief Create a User in the in the database.
     * 
     * @param username 
     * @param password 
     * @param initialBalance 
     */
    void createUser(string username, string password, int initialBalance);

    /**
     * @brief Checks if the login credintials are correct.
     * 
     * @param username 
     * @param password 
     */
    void login(string username, string password);

    /**
     * @brief Changes password in the database.
     * 
     * @param username 
     * @param newPassword 
     */
    void changePassword(string username, string newPassword); 

    /**
     * @brief Get the Product Details from the database.
     * 
     * @param userWants 
     */
    void getProductDetails(string userWants);
    
    /**
     * @brief Get the Product Names from the database.
     * 
     * @param type 
     */
    void getProductNames(string type);

    /**
     * @brief Buys a product from the database.
     * 
     * @param username 
     * @param productName 
     * @param qauntity 
     */
    void buy(string username, string productName, int qauntity);
    
    /**
     * @brief Adds money to user's account.
     * 
     * @param username 
     * @param amount 
     */
    void addMoney(string username, int amount);

    /**
     * @brief Get the Orders of an user.
     * 
     * @param username 
     */
    void getOrders(string username);
    
    /**
     * @brief Get the account balance of an user.
     * 
     * @param username 
     */
    void getBalance(string username);

    /**
     * @brief Destroy the Operation object
     * Closes the database connection object if it openned it.
     * Does not close the connection object incase it was a shared one.
     */
    ~Operation();
};

#endif