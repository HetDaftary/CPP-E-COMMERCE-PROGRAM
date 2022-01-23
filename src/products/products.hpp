#include <iostream>
#include <cstdio>

#define bufferSize 512

using std::string;
using std::ostream;
using std::to_string;

class Product {
public:
    enum ProductType {
        smartphone, laptop
    };
    static string typeNameString[];
protected:
    string productName, countryOfOrigin;
    int price, stock;
    ProductType type;
public:
    Product(string productName, string countryOfOrigin, int price, int stock, ProductType type) {
        this->productName = productName;
        this->countryOfOrigin = countryOfOrigin;
        this->price = price;
        this->stock = stock;
        this->type = type;
    }

    string toStr() {
        string result = productName + ":\n";
        result += "\tCountry of origin: " + countryOfOrigin + "\n";
        result += "\tPrice: " + to_string(price) + "\n";
        result += "\tStock: " + to_string(stock) + "\n";
        result += "\tType: " + typeNameString[type] + "\n";

        return result;
    }

    string getProductName() {
        return productName;
    }

    // This method needs to be defined in every derived class.
    // Concept of interface is used here.
    virtual char* getSQLInsertStatement() = 0;
};

string Product::typeNameString[] = {"smartphone", "laptop"};

class Smartphone : public Product {
    int ram, rom, numberOfCameras;
    string processor;
public:
    Smartphone(string productName, string countryOfOrigin, int price, int stock, int numberOfCameras, string processor, int ram, int rom) : Product(productName, countryOfOrigin, price, stock, smartphone) {
        this->numberOfCameras = numberOfCameras;
        this->processor = processor;
        this->ram = ram;
        this->rom = rom;
    }

    string toStr() {
        string result = Product::toStr();
        result += "\tNumber of cameras: " + to_string(numberOfCameras) + "\n";
        result += "\tProcessor: " + processor + "\n";
        result += "\tRAM: " + to_string(ram) + "\n";
        result += "\tROM: " + to_string(rom) + "\n";
        return result;
    }

    char* getSQLInsertStatement() {
        char* syntax = (char*)"INSERT INTO ProductDetails('productName', 'countryOfOrigin', 'price', 'stock', 'type', 'processor', 'ram', 'rom', 'numberOfCameras') VALUES ('%s', '%s', %d, %d, %d, '%s', %d, %d, %d); ";
        char* toReturn = new char[bufferSize];
        sprintf(toReturn, syntax, productName.c_str(), countryOfOrigin.c_str(), price, stock, smartphone, processor.c_str(), ram, rom, numberOfCameras);
        return toReturn;
    }
};

class Laptop : public Product {
    int ram, rom, hasTouchScreen;
public:
    Laptop(string productName, string countryOfOrigin, int price, int stock, int ram, int rom, int hasTouchScreen) : Product(productName, countryOfOrigin, price, stock, laptop) {
        this->ram = ram;
        this->rom = rom;
        this->hasTouchScreen = hasTouchScreen;
    }

    string toStr() {
        string result = Product::toStr();
        result += "\tRAM: " + to_string(ram) + "\n";
        result += "\tROM: " + to_string(rom) + "\n";
        result += "\tHas touch screen: " + to_string(hasTouchScreen) + "\n";
        return result;
    }

    char* getSQLInsertStatement() {
        char* syntax = (char*)"INSERT INTO ProductDetails('productName', 'countryOfOrigin', 'price', 'stock', 'type', 'ram', 'rom', 'hasTouchScreen') VALUES('%s', '%s', %d, %d, %d, %d, %d, %d);";
        char* toReturn = new char[bufferSize];
        sprintf(toReturn, syntax, productName.c_str(), countryOfOrigin.c_str(), price, stock, laptop, ram, rom, hasTouchScreen);
        return toReturn;
    }
};  