#include "../include/operation.hpp"


#include "../include/logger.hpp"
#include "../include/products.hpp"

static string seperator = ",";
string ordersFileName = "data/orders.txt";
mutex ordersFileMutex;

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

typedef struct {
    vector<vector<char*>> dataStr;
    vector<vector<int>> dataInt;
} SQLData;
/**
 * @brief Will mainly be useful for SELECT queries.
 * Gets x rows with m string columns and n int columns.
 * NOTE: User needs to write query in a way that first m string columns are recevied and then n int columns.
 * 
 * @param db 
 * @param query 
 * @param numberOfColumns 
 * @return SQLData 
 */
SQLData getDataFromSQL(sqlite3* db, string query, int numberOfStrColumns, int numberOfIntColumns) {
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
 * @param db 
 * @param query 
 * @return char* errorMsg.
 */
char* putDataInDatabase(sqlite3* db, string query) {
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

Operation::Operation() {
    sqlite3_open(Logger::databaseFileName.c_str(), &db);
    isSharedConnection = false;
}

Operation::Operation(sqlite3* db) {
    this->db = db;
    isSharedConnection = true;
}

char* Operation::getResponse() {
    return response;
} 

void Operation::createUser(string username, string password, int initialBalance) {
    // Write insert query here.
    string sql = "INSERT INTO Users (username, password, balance) VALUES ('" + username + "', '" + password + "', " + to_string(initialBalance) + ");";

    char *zErrMsg = putDataInDatabase(db, sql);

    if (zErrMsg) {
        // If User exists, this will be printed.
        response = (char*)"0";
    } else {
        response = (char*)"1";
    }
}

void Operation::login(string username, string password) {
    // Write select query here.
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

void Operation::changePassword(string username, string newPassword) {
        // Write update query here.
    string sql = "UPDATE Users SET password = '" + newPassword + "' WHERE username = '" + username + "';";

    char* zErrMsg = putDataInDatabase(db, sql);

    response = (char*)"Password changed successfully.";
    // We do not need to worry about this because the client's username will always be correct when calling this.
    // This statement will throw errors when the username is not found.
}

void Operation::getProductDetails(string userWants) {
    string sql;
    int columnNumbers[2];
    // Firstly, we will ask for all the string columns and then for int columns.

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

    SQLData data = getDataFromSQL(db, sql, columnNumbers[0], columnNumbers[1]);
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

    vector<vector<char*>> data = getDataFromSQL(db, sql, 1, 0).dataStr;
    for (int i = 0; i < data.size(); i++) {
        products.push_back(data[i][0]);
    }

    string toSend = join("\n", products);
    response = new char[toSend.size()];
    strcpy(response, toSend.c_str());        
}

void Operation::buy(string username, string productName, int qauntity) {
    // Write select query here.
    char *zErrMsg = 0;
    int currentStock, price, rc;
    string sql;

    try {    
        string sql = "SELECT stock,price FROM ProductDetails WHERE productName = '" + productName + "';";

        vector<vector<int>> data = getDataFromSQL(db, sql, 0, 1).dataInt;

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

    // Check balance of user now.
    sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

    int balance = getDataFromSQL(db, sql, 0, 1).dataInt[0][0];
    // Check for the price of the product from laptopDetails or smartphoneDetails 
    // and deduct the price from balance.

    if (balance >= qauntity * price) {
        balance -= qauntity * price;
    
        sql = "UPDATE Users SET balance = " + to_string(balance) + " WHERE username = '" + username + "';";
        putDataInDatabase(db, sql);

        sql = "UPDATE ProductDetails SET stock = stock - " + to_string(qauntity) + " WHERE productName = '" + productName + "';";
        putDataInDatabase(db, sql);
            
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

void Operation::addMoney(string username, int amount) {
    // Update User balance and add amount.
        
    string sql = "UPDATE Users SET balance = balance + " + to_string(amount) + " WHERE username = '" + username + "';";

    putDataInDatabase(db, sql);
        
    response = (char*)"Added money";
}

void Operation::getOrders(string username) {
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

void Operation::getBalance(string username) {
    char* zErrMsg = 0;
    string sql = "SELECT balance FROM Users WHERE username = '" + username + "';";

    int balance = getDataFromSQL(db, sql, 0, 1).dataInt[0][0];

    string balanceStr = to_string(balance);
    response = new char[balanceStr.length() + 1];
    strcpy(response, balanceStr.c_str());
    // I am doing this because it throws garbage value otherwise.
    // c_str returns the char* used by this local string object.
    // This does not send a copy of the string.
    // So, it gets freed after the completion of this function.
}

Operation::~Operation() {
    if (!isSharedConnection) {
        sqlite3_close(db);
    }
}