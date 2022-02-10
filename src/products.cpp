#include "../include/products.hpp"

using std::string;
using std::ostream;
using std::to_string;

string Product::typeNameString[] = {"smartphone", "laptop"};

/**
 * @brief Construct a new Product object
 * 
 * @param productName 
 * @param countryOfOrigin 
 * @param price 
 * @param stock 
 * @param type 
 */
Product::Product(string productName, string countryOfOrigin, int price, int stock, ProductType type) {
    this->productName = productName;
    this->countryOfOrigin = countryOfOrigin;
    this->price = price;
    this->stock = stock;
    this->type = type;
}

/**
 * @brief Construct a new Product object
 * 
 * @param productName 
 * @param countryOfOrigin 
 * @param price 
 * @param stock 
 * @param type 
 */
Product::Product(char* productName, char* countryOfOrigin, int price, int stock, ProductType type) {
    this->productName = string(productName);
    this->countryOfOrigin = string(countryOfOrigin);
    this->price = price;
    this->stock = stock;
    this->type = type;
}

/**
 * @brief Converts the product info to a string.
 * 
 * @return string 
 */
string Product::toStr() {
    string result = productName + ":\n";
    result += "\tCountry of origin: " + countryOfOrigin + "\n";
    result += "\tPrice: " + to_string(price) + "\n";
    result += "\tStock: " + to_string(stock) + "\n";
    result += "\tType: " + typeNameString[type] + "\n";

    return result;
}

/**
 * @brief Get the Product Name.
 * 
 * @return string 
 */
string Product::getProductName() {
    return productName;
}

/**
 * @brief Construct a new Smartphone object
 * 
 * @param productName 
 * @param countryOfOrigin 
 * @param price 
 * @param stock 
 * @param numberOfCameras 
 * @param processor 
 * @param ram 
 * @param rom 
 */
Smartphone::Smartphone(string productName, string countryOfOrigin, int price, int stock, int numberOfCameras, string processor, int ram, int rom) : Product(productName, countryOfOrigin, price, stock, smartphone) {
    this->numberOfCameras = numberOfCameras;
    this->processor = processor;
    this->ram = ram;
    this->rom = rom;
}

/**
 * @brief Construct a new Smartphone object
 * 
 * @param productName 
 * @param countryOfOrigin 
 * @param price 
 * @param stock 
 * @param numberOfCameras 
 * @param processor 
 * @param ram 
 * @param rom 
 */
Smartphone::Smartphone(char* productName, char* countryOfOrigin, int price, int stock, int numberOfCameras, char* processor, int ram, int rom) : Product(productName, countryOfOrigin, price, stock, smartphone) {
    this->numberOfCameras = numberOfCameras;
    this->processor = string(processor);
    this->ram = ram;
    this->rom = rom;
}

/**
 * @brief Converts the smartphone info to a string.
 * 
 * @return string 
 */
string Smartphone::toStr() {
    string result = Product::toStr();
    result += "\tNumber of cameras: " + to_string(numberOfCameras) + "\n";
    result += "\tProcessor: " + processor + "\n";
    result += "\tRAM: " + to_string(ram) + "\n";
    result += "\tROM: " + to_string(rom) + "\n";
    return result;
}

/**
 * @brief This returns the sqlite insert query.
 * 
 * @return char* 
 */
char* Smartphone::getSQLInsertStatement() {
    char* syntax = (char*)"INSERT INTO ProductDetails('productName', 'countryOfOrigin', 'price', 'stock', 'type', 'processor', 'ram', 'rom', 'numberOfCameras') VALUES ('%s', '%s', %d, %d, %d, '%s', %d, %d, %d); ";
    char* toReturn = new char[bufferSize];
    sprintf(toReturn, syntax, productName.c_str(), countryOfOrigin.c_str(), price, stock, smartphone, processor.c_str(), ram, rom, numberOfCameras);
    return toReturn;
}

/**
 * @brief Construct a new Laptop object
 * 
 * @param productName 
 * @param countryOfOrigin 
 * @param price 
 * @param stock 
 * @param ram 
 * @param rom 
 * @param hasTouchScreen 
 */
Laptop::Laptop(string productName, string countryOfOrigin, int price, int stock, int ram, int rom, int hasTouchScreen) : Product(productName, countryOfOrigin, price, stock, laptop) {
    this->ram = ram;
    this->rom = rom;
    this->hasTouchScreen = hasTouchScreen;
}


Laptop::Laptop(char* productName, char* countryOfOrigin, int price, int stock, int ram, int rom, int hasTouchScreen) : Product(productName, countryOfOrigin, price, stock, laptop) {
    this->ram = ram;
    this->rom = rom;
    this->hasTouchScreen = hasTouchScreen;
}

/**
 * @brief Converts the smartphone info to a string.
 * 
 * @return string : The string representation of the laptop object.
 */
string Laptop::toStr() {
    string result = Product::toStr();
    result += "\tRAM: " + to_string(ram) + "\n";
    result += "\tROM: " + to_string(rom) + "\n";
    result += "\tHas touch screen: " + to_string(hasTouchScreen) + "\n";
    return result;
}

/**
 * @brief This returns the sqlite insert query.
 * 
 * @return char* : The sqlite insert query.
 */
char* Laptop::getSQLInsertStatement() {
    char* syntax = (char*)"INSERT INTO ProductDetails('productName', 'countryOfOrigin', 'price', 'stock', 'type', 'ram', 'rom', 'hasTouchScreen') VALUES('%s', '%s', %d, %d, %d, %d, %d, %d);";
    char* toReturn = new char[bufferSize];
    sprintf(toReturn, syntax, productName.c_str(), countryOfOrigin.c_str(), price, stock, laptop, ram, rom, hasTouchScreen);
    return toReturn;
}