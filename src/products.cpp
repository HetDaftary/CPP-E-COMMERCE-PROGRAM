#include "../include/products.hpp"

string Product::typeNameString[] = {"smartphone", "laptop"};

Product::Product(string productName, string countryOfOrigin, int price, int stock, ProductType type) {
    this->productName = productName;
    this->countryOfOrigin = countryOfOrigin;
    this->price = price;
    this->stock = stock;
    this->type = type;
}

Product::Product(char* productName, char* countryOfOrigin, int price, int stock, ProductType type) {
    this->productName = string(productName);
    this->countryOfOrigin = string(countryOfOrigin);
    this->price = price;
    this->stock = stock;
    this->type = type;
}

string Product::toStr() {
    string result = productName + ":\n";
    result += "\tCountry of origin: " + countryOfOrigin + "\n";
    result += "\tPrice: " + to_string(price) + "\n";
    result += "\tStock: " + to_string(stock) + "\n";
    result += "\tType: " + typeNameString[type] + "\n";

    return result;
}

string Product::getProductName() {
    return productName;
}

Smartphone::Smartphone(string productName, string countryOfOrigin, int price, int stock, int numberOfCameras, string processor, int ram, int rom) : Product(productName, countryOfOrigin, price, stock, smartphone) {
    this->numberOfCameras = numberOfCameras;
    this->processor = processor;
    this->ram = ram;
    this->rom = rom;
}

Smartphone::Smartphone(char* productName, char* countryOfOrigin, int price, int stock, int numberOfCameras, char* processor, int ram, int rom) : Product(productName, countryOfOrigin, price, stock, smartphone) {
    this->numberOfCameras = numberOfCameras;
    this->processor = string(processor);
    this->ram = ram;
    this->rom = rom;
}

string Smartphone::toStr() {
    string result = Product::toStr();
    result += "\tNumber of cameras: " + to_string(numberOfCameras) + "\n";
    result += "\tProcessor: " + processor + "\n";
    result += "\tRAM: " + to_string(ram) + "\n";
    result += "\tROM: " + to_string(rom) + "\n";
    return result;
}

char* Smartphone::getSQLInsertStatement() {
    char* syntax = (char*)"INSERT INTO ProductDetails('productName', 'countryOfOrigin', 'price', 'stock', 'type', 'processor', 'ram', 'rom', 'numberOfCameras') VALUES ('%s', '%s', %d, %d, %d, '%s', %d, %d, %d); ";
    char* toReturn = new char[bufferSize];
    sprintf(toReturn, syntax, productName.c_str(), countryOfOrigin.c_str(), price, stock, smartphone, processor.c_str(), ram, rom, numberOfCameras);
    return toReturn;
}

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

string Laptop::toStr() {
    string result = Product::toStr();
    result += "\tRAM: " + to_string(ram) + "\n";
    result += "\tROM: " + to_string(rom) + "\n";
    result += "\tHas touch screen: " + to_string(hasTouchScreen) + "\n";
    return result;
}

char* Laptop::getSQLInsertStatement() {
    char* syntax = (char*)"INSERT INTO ProductDetails('productName', 'countryOfOrigin', 'price', 'stock', 'type', 'ram', 'rom', 'hasTouchScreen') VALUES('%s', '%s', %d, %d, %d, %d, %d, %d);";
    char* toReturn = new char[bufferSize];
    sprintf(toReturn, syntax, productName.c_str(), countryOfOrigin.c_str(), price, stock, laptop, ram, rom, hasTouchScreen);
    return toReturn;
}