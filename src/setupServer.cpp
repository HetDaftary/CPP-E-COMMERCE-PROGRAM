/**
 * @file setupDatabase.cpp
 * @brief We just setup the database tables here So our server has all the info to run.
 * @version 0.1
 * @date 2022-01-16
 *
 */

#include "sqlite3.h"
#include "Logger/logger.hpp"
#include <iostream>
#include <vector>
#include "sqlite3.h"
#include "products/products.hpp"
#include <random>
#include <fstream>

#define DEBUG 0

using std::string;
using std::vector;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;
using std::ofstream;

const std::string countries[] = {"India", "USA", "Taiwan"};
const std::string smartphoneNames[] = {"samsung_a21", "samsung_m31", "lava_Z1"};
const std::string laptopNames[] = {"lenovo_yoga", "hp_pavalilion", "dell"};

#define numberOfSmartphones 3
#define numberOfLaptops 3
#define numberOfCountries 3

string createTableSyntax[] = {
    "CREATE TABLE IF NOT EXISTS 'ProductDetails' ('productName' TEXT, 'countryOfOrigin' TEXT, 'price' INTEGER, 'ram' INTEGER, 'rom' INTEGER, 'hasTouchScreen' INTEGER, 'numberOfCameras' INTEGER, 'stock' INTEGER, 'processor'	TEXT, 'type' INTEGER, PRIMARY KEY('productName'));",
    "CREATE TABLE IF NOT EXISTS 'Users' ('username' TEXT, 'password' TEXT, 'balance' INTEGER, PRIMARY KEY('username'));"  
};
string ordersFileName = "data/orders.txt";

random_device rd;
mt19937 mt(rd());
uniform_int_distribution<int> smartphoneDistribution(0, numberOfSmartphones - 1);
uniform_int_distribution<int> laptopDistribution(0, numberOfLaptops - 1);
uniform_int_distribution<int> countryDistribution(0, numberOfCountries - 1);
uniform_int_distribution<int> ramDistribution(1, 4);
// We will multiply this with 4 to get ram details like 4, 8, 12, 16.
// We would also use this ramDistribtion for rom but multiply by 8 for smartphone and 256 for laptops.
// And we will also use for for number of cameras but has it is.
uniform_int_distribution<int> hasTouchScreenDistribution(0, 2);
uniform_int_distribution<int> priceDistribution(10000, 20001);
uniform_int_distribution<int> stockDistribution(10, 100);

/**
 * @brief Get the Smartphone Details 
 * 
 * @param products 
 */
void getSmartphoneDetails(vector<Product*>& products) {
    for (int i = 0; i < numberOfSmartphones; i++) {
        string productName = smartphoneNames[i], processor = "Snapdragon", countryOfOrigin = countries[countryDistribution(mt)];
        int price = priceDistribution(mt), ram = 4 * ramDistribution(mt), rom = 8 * ramDistribution(mt), numberOfCameras = ramDistribution(mt), stock = stockDistribution(mt);

        Product* s = new Smartphone(productName, countryOfOrigin, price, stock, numberOfCameras, processor, ram, rom);        

        products.push_back(s);
    }
}

/**
 * @brief Get the Laptop Details
 * 
 * @param products 
 */
void getLaptopDetails(vector<Product*>& products) {
    vector<Product*> laptops;
    for (int i = 0; i < numberOfSmartphones; i++) {
        string productName = laptopNames[i], countryOfOrigin = countries[countryDistribution(mt)];
        int price = priceDistribution(mt), ram = 4 * ramDistribution(mt), rom = 256 * ramDistribution(mt), stock = stockDistribution(mt), hasTouchScreen = hasTouchScreenDistribution(mt);

        Product* l = new Laptop(productName, countryOfOrigin, price, stock, ram, rom, hasTouchScreen);
        
        products.push_back(l);
    }
}

/**
 * @brief The main function that setup the stuff necessary to start the server.
 * 
 * @return int 
 */
int main() {
    Logger::EnableFileOutput();
    sqlite3* db;
    char* errorMsg;

    sqlite3_open(databaseFileName.c_str(), &db);
    // This file name is mentioned in Logger.

    // Creating tables.
    for (string sql : createTableSyntax) {
        sqlite3_exec(db, sql.c_str(), NULL, NULL, &errorMsg);
        if (errorMsg != NULL) {
            Logger::Critical(errorMsg);
            errorMsg = NULL;
        }
    }

    // These functions Laptop and Smartphone details in the productDetails vector.
    vector<Product*> productDetails;
    getLaptopDetails(productDetails);
    getSmartphoneDetails(productDetails);

    // Inserting products to the database.
    for (Product* p : productDetails) {
        sqlite3_exec(db, p->getSQLInsertStatement(), NULL, NULL, &errorMsg);

        if (errorMsg) {
            Logger::Info(errorMsg);
            errorMsg = NULL;
        }
    }

    ofstream ordersFile;
    ordersFile.open(ordersFileName);

    ordersFile << "username,productName,quantity,price\n";

    ordersFile.close();

    sqlite3_close(db);
    Logger::CloseFileOutput();
    return 0;
}