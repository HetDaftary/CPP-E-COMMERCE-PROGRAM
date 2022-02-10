#include "../include/operation.hpp"

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

static string seperator = ",";
string ordersFileName = "data/orders.txt";
mutex ordersFileMutex;
const string Operation::databaseFileName = "data/database.db";

/**
     * @brief Will mainly be useful for SELECT queries.
     * Gets x rows with m string columns and n int columns.
     * NOTE: User needs to write query in a way that first m string columns are recevied and then n int columns.
     * 
     * @param query : string
     * @param numberOfColumns : int 
     * @return SQLData : SQLData object
     */
Operation::SQLData Operation::getDataFromSQL(string query, int numberOfStrColumns, int numberOfIntColumns) {
    SQLData data;
    
    char* zErrMsg = 0;
    int rc, i;
    
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        Logger::Error("SQL error: %s\nWas Running query: \"%s\"", sqlite3_errmsg(db), query.c_str());
        sqlite3_finalize(stmt);
        return data;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        vector<char*> row;
        vector<int> rowInt;
        for (i = 0; i < numberOfStrColumns; i++) {
            char* val = (char*)sqlite3_column_text(stmt, i);
            if (val) {    
                char* toPush = new char[strlen(val) + 1];
                strcpy(toPush, val);
                row.push_back(toPush);
            } else {
                row.push_back(NULL);
            }
        }
        for (i = 0; i < numberOfIntColumns; i++) {
            rowInt.push_back(sqlite3_column_int(stmt, numberOfStrColumns + i));
        }
        data.dataStr.push_back(row);
        data.dataInt.push_back(rowInt);
    }

    sqlite3_finalize(stmt);
    return data;
}

/**
 * @brief To update database.
 * Mainly for Insert and Update queries.
 * 
 * @param query : string
 * @return char* errorMsg.
 */
char* Operation::putDataInDatabase(string query) {
    sqlite3_stmt* stmt;
    
    char* errorMsg;
    int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errorMsg);
    
    if (rc != SQLITE_OK) {
        Logger::Error("SQL error: %s\nWas Running query: \"%s\"", errorMsg, query.c_str());
        sqlite3_free(errorMsg);
        return errorMsg;
    }

    return errorMsg;
}

/**
 * @brief Construct a new Operation object
 * Creates it's own connection to the database.
 * To be used when SQLITE is in MULTI-THREADED mode.
 */
Operation::Operation() {
    sqlite3_open(Operation::databaseFileName.c_str(), &db);
    isSharedConnection = false;
}

/**
 * @brief Construct a new Operation object
 * Gets the shared database object.
 * To be used when SQLITE is in SERIALIZED mode.
 * @param db : sqlite3*
 */
Operation::Operation(sqlite3* db) {
    this->db = db;
    isSharedConnection = true;
}

/**
 * @brief Get the Response object
 * 
 * @return char* response
 */
char* Operation::getResponse() {
    return response;
} 

/**
 * @brief Create a User in the in the database.
 * 
 * @param username : string
 * @param password : string 
 * @param initialBalance : int
 */
void Operation::createUser(string username, string password, int initialBalance) {
    //! Write insert query here.
    string sql = "INSERT INTO Users (username, password, balance) VALUES ('" + username + "', '" + password + "', " + to_string(initialBalance) + ");";

    char *zErrMsg = putDataInDatabase(sql);

    if (zErrMsg) {
        //! If User exists, this will be printed.
        response = (char*)"0";
    } else {
        response = (char*)"1";
    }
}

/**
 * @brief Checks if the login credintials are correct.
 * 
 * @param username : string
 * @param password : string
 */
void Operation::login(string username, string password) {
    //! Write select query here.
    string sql = "SELECT password from Users WHERE username='" + username + "';";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        char* passwordFromDB = (char*)sqlite3_column_blob(stmt, 0);
        if (password == string(passwordFromDB)) {
            response = (char*)"1";
        } else {
            response = (char*)"0";
        }
    } else {
        response = (char*)"0";
    }
}

/**
 * @brief Changes password in the database.
 * 
 * @param username : string
 * @param newPassword : string 
 */
void Operation::changePassword(string username, string newPassword) {
        //! Write update query here.
    string sql = "UPDATE Users SET password = '" + newPassword + "' WHERE username = '" + username + "';";

    char* zErrMsg = putDataInDatabase(sql);

    response = (char*)"Password changed successfully.";
    //! We do not need to worry about this because the client's username will always be correct when calling this.
    //! This statement will throw errors when the username is not found.
}

/**
 * @brief Get the Product Details from the database.
 * 
 * @param userWants : string 
 */
void Operation::getProductDetails(string userWants) {
    string sql;
    int columnNumbers[2];
    //! Firstly, we will ask for all the string columns and then for int columns.

    if (userWants == "all") {
        sql = "SELECT productName,countryOfOrigin,processor,price,ram,rom,hasTouchScreen,numberOfCameras,stock,type FROM ProductDetails WHERE stock > 0;";
        columnNumbers[0] = 3;
        columnNumbers[1] = 7;
    } else if (userWants == "laptop") {
        sql = "SELECT productName,countryOfOrigin,price,ram,rom,hasTouchScreen,stock FROM ProductDetails WHERE stock > 0 AND type = " + to_string(Product::laptop) + ";";
        columnNumbers[0] = 2;
        columnNumbers[1] = 5;
    } else if (userWants == "smartphome") {
        sql = "SELECT productName,countryOfOrigin,price,ram,rom,numberOfCameras,stock FROM ProductDetails WHERE stock > 0 AND type = " + to_string(Product::smartphone) + ";";
        columnNumbers[0] = 3;
        columnNumbers[1] = 5;
    } else {
        sql = "SELECT productName,countryOfOrigin,processor,price,ram,rom,hasTouchScreen,numberOfCameras,stock,type FROM ProductDetails WHERE stock > 0 AND productName = '" + userWants + "' COLLATE NOCASE;";
        columnNumbers[0] = 3;
        columnNumbers[1] = 7;
    }

    SQLData data = getDataFromSQL(sql, columnNumbers[0], columnNumbers[1]);
    vector<vector<char*>> dataStr = data.dataStr;
    vector<vector<int>> dataInt = data.dataInt;

    vector<string> productDetails;
    int i;
    string toPush;

    if (data.dataStr.size() == 0) {
        response = (char*)"Product not found.";
        return;
    } else if (userWants == "laptop") {
        for (i = 0; i < dataStr.size(); i++) {
            toPush = Laptop(string(dataStr[i][0]), string(dataStr[i][1]), dataInt[i][0], dataInt[i][4], dataInt[i][1], dataInt[i][2], dataInt[i][3]).toStr();
            productDetails.push_back(toPush);
        }
    } else if (userWants == "smartphone") {
        for (i = 0; i < dataStr.size(); i++) {
            toPush = Smartphone(dataStr[i][0], dataStr[i][1], dataInt[i][0], dataInt[i][4], dataInt[i][3], dataStr[i][2], dataInt[i][1], dataInt[i][2]).toStr();
            productDetails.push_back(toPush);
        }
    } else {
        for (i = 0; i < dataStr.size(); i++) {
            if (dataInt[i][6] == Product::smartphone) {
                toPush = Smartphone(dataStr[i][0], dataStr[i][1], dataInt[i][0], dataInt[i][5], dataInt[i][4], dataStr[i][2], dataInt[i][1], dataInt[i][2]).toStr();
            } else {
                toPush = Laptop(dataStr[i][0], dataStr[i][1], dataInt[i][0], dataInt[i][5], dataInt[i][1], dataInt[i][2], dataInt[i][3]).toStr();
            }
            productDetails.push_back(toPush);
        }
    }

    string toSend = join("\n", productDetails);
    response = new char[toSend.size()];
    strcpy(response, toSend.c_str());
}

/**
 * @brief Get the Product Names from the database.
 * 
 * @param type : string 
 */
void Operation::getProductNames(string type) {
    string sql, productName;
    vector<string> products;
        
    if (type == "all") {
        sql = "SELECT productName FROM ProductDetails WHERE stock > 0;";
    } else if (type == "smartphone") {
        sql = "SELECT productName FROM ProductDetails WHERE type = " + to_string(Product::smartphone) + " AND stock > 0;";
    } else if (type == "laptop") {
        sql = "SELECT productName FROM ProductDetails WHERE type = " + to_string(Product::laptop) + " AND stock > 0;";
    } else {
        response = (char*) "Wrong type entered";
        return;
    }

    vector<vector<char*>> data = getDataFromSQL(sql, 1, 0).dataStr;
    for (int i = 0; i < data.size(); i++) {
        products.push_back(data[i][0]);
    }

    string toSend = join("\n", products);
    response = new char[toSend.size()];
    strcpy(response, toSend.c_str());        
}

/**
 * @brief Buys a product from the database.
 * 
 * @param username : string
 * @param productName : string
 * @param qauntity : int
 */
void Operation::buy(string username, string productName, int qauntity) {
    //! Write select query here.
    char *zErrMsg = 0;
    int currentStock, price, rc;
    string sql;

    try {    
        string sql = "SELECT stock,price FROM ProductDetails WHERE productName = '" + productName + "' COLLATE NOCASE;";

        vector<vector<int>> data = getDataFromSQL(sql, 0, 1).dataInt;

        if (data.size() == 0) {
            throw (char*)"Product not found.";
        }

        currentStock = data[0][0];
        price = data[0][1];

        if (currentStock < qauntity) {
            throw (char*) "Not enough stock";
        }
    } catch (char* error) {
        response = error;
    }

    //! Check balance of user now.
    sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

    int balance = getDataFromSQL(sql, 0, 1).dataInt[0][0];
    //! Check for the price of the product from laptopDetails or smartphoneDetails 
    //! and deduct the price from balance.

    if (balance >= qauntity * price) {
        balance -= qauntity * price;
    
        sql = "UPDATE Users SET balance = " + to_string(balance) + " WHERE username = '" + username + "';";
        putDataInDatabase(sql);

        sql = "UPDATE ProductDetails SET stock = stock - " + to_string(qauntity) + " WHERE productName = '" + productName + "';";
        putDataInDatabase(sql);
            
        response = (char*) "Success";

        //! We need to append this order to data/orders.txt
        vector<string> partsToAppend = {username, productName, to_string(qauntity), to_string(price)};
        string order = join(seperator, partsToAppend);
        //!std::cout << "Order is " << order << std::endl;
        //!std::cout << username << " " << productName << " " << qauntity << " " << price << std::endl;
        
        ordersFileMutex.lock();
        //! Making our write operation thread safe.
        //! Only one thread should be able to access to the file at a time.
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
 * @param username : string
 * @param amount : int
 */
void Operation::addMoney(string username, int amount) {
    //! Update User balance and add amount.
        
    string sql = "UPDATE Users SET balance = balance + " + to_string(amount) + " WHERE username = '" + username + "';";

    putDataInDatabase(sql);
        
    response = (char*)"Added money";
}

/**
 * @brief Get the Orders of an user.
 * 
 * @param username : string
 */
void Operation::getOrders(string username) {
    //! A user can access his/her orders.
    vector<string> orders;
    orders.push_back("Username,Product Name,Quantity,Price");

    ordersFileMutex.lock();
    //! Making our write operation thread safe.
    //! Only one thread should be able to access to the file at a time.
        
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
    //!std::cout << ans;
    response = new char[ans.length() + 1];
    strcpy(response, ans.c_str());
    //! I am doing this because it throws garbage value otherwise.
    //! c_str returns the char* used by this local string object.
    //! This does not send a copy of the string.
    //! So, it gets freed after the completion of this function.
    //!std::cout << response << endl;
}

/**
 * @brief Get the account balance of an user.
 * 
 * @param username : string
 */
void Operation::getBalance(string username) {
    char* zErrMsg = 0;
    string sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

    int balance = getDataFromSQL(sql, 0, 1).dataInt[0][0];

    string balanceStr = to_string(balance);
    response = new char[balanceStr.length() + 1];
    strcpy(response, balanceStr.c_str());
    //! I am doing this because it throws garbage value otherwise.
    //! c_str returns the char* used by this local string object.
    //! This does not send a copy of the string.
    //! So, it gets freed after the completion of this function.
}

/**
 * @brief Destroy the Operation object
 * Closes the database connection object if it openned it.
 * Does not close the connection object incase it was a shared one.
 */
Operation::~Operation() {
    if (!isSharedConnection) {
        sqlite3_close(db);
    }
}