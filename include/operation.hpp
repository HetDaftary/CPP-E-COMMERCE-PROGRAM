#ifndef _OPERATION_HPP
#define _OPERATION_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <mutex>
#include <sqlite3.h>
#include "../include/splitAndJoin.hpp"
#include <cstring>
#include "../include/logger.hpp"
#include "../include/products.hpp"

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
        std::vector<std::vector<char*>> dataStr;
        std::vector<std::vector<int>> dataInt;
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
    SQLData getDataFromSQL(std::string query, int numberOfStrColumns, int numberOfIntColumns);

    /**
     * @brief To update database.
     * Mainly for Insert and Update queries.
     * 
     * @param query 
     * @return char* errorMsg.
     */
    char* putDataInDatabase(std::string query);
public:
    static const std::string databaseFileName;
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
     * @param db : sqlite3*
     */
    Operation(sqlite3* db);

    /**
     * @brief Get the Response object
     * 
     * @return char* : response
     */
    char* getResponse();

    /**
     * @brief Create a User in the in the database.
     * 
     * @param username : string
     * @param password : string 
     * @param initialBalance : int
     */
    void createUser(std::string username, std::string password, int initialBalance);

    /**
     * @brief Checks if the login credintials are correct.
     * 
     * @param username : string
     * @param password : string
     */
    void login(std::string username, std::string password);

    /**
     * @brief Changes password in the database.
     * 
     * @param username : string
     * @param newPassword : string
     */
    void changePassword(std::string username, std::string newPassword); 

    /**
     * @brief Get the Product Details from the database.
     * 
     * @param userWants : string
     */
    void getProductDetails(std::string userWants);
    
    /**
     * @brief Get the Product Names from the database.
     * 
     * @param type : string
     */
    void getProductNames(std::string type);

    /**
     * @brief Buys a product from the database.
     * 
     * @param username : string 
     * @param productName : string
     * @param qauntity : int
     */
    void buy(std::string username, std::string productName, int qauntity);
    
    /**
     * @brief Adds money to user's account.
     * 
     * @param username : string
     * @param amount : int
     */
    void addMoney(std::string username, int amount);

    /**
     * @brief Get the Orders of an user.
     * 
     * @param username : string
     */
    void getOrders(std::string username);
    
    /**
     * @brief Get the account balance of an user.
     * 
     * @param username : string
     */
    void getBalance(std::string username);

    /**
     * @brief Destroy the Operation object
     * Closes the database connection object if it openned it.
     * Does not close the connection object incase it was a shared one.
     */
    ~Operation();
};

#endif