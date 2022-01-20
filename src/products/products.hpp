#include <iostream>

using std::string;
using std::ostream;
using std::to_string;

class Product {
protected:
    string productName, countryOfOrigin;
    int price;
public:
    Product(string productName, string countryOfOrigin, int price) {
        this->productName = productName;
        this->countryOfOrigin = countryOfOrigin;
        this->price = price;
    }

    string to_str() {
        string result = productName + ":\n";
        result += "\tCountry of origin: " + countryOfOrigin + "\n";
        result += "\tPrice: " + to_string(price) + "\n";
        return result;
    }

    string getProductName() {
        return productName;
    }

    // This method needs to be defined in every derived class.
    // Concept of interface is used here.
    virtual string getSQLInsertStatement() = 0;
};

class Smartphone : public Product {
    int ram, rom, numberOfCameras;
    string processor;
public:
    Smartphone(string productName, string countryOfOrigin, int price, int numberOfCameras, string processor, int ram, int rom) : Product(productName, countryOfOrigin, price) {
        this->numberOfCameras = numberOfCameras;
        this->processor = processor;
        this->ram = ram;
        this->rom = rom;
    }

    string to_str() {
        string result = Product::to_str();
        result += "\tNumber of cameras: " + to_string(numberOfCameras) + "\n";
        result += "\tProcessor: " + processor + "\n";
        result += "\tRAM: " + to_string(ram) + "\n";
        result += "\tROM: " + to_string(rom) + "\n";
        return result;
    }

    string getSQLInsertStatement() {
        return "INSERT INTO \"SmartphoneDetails\" (productName, countryOfOrigin, price, numberOfCameras, processor, ram, rom) VALUES (\""+ string(productName) + "\",\"" + string(countryOfOrigin) + string("\",") + to_string(price) + "," + to_string(numberOfCameras) + ",\"" + string(processor) + "\"," + to_string(ram) + "," + to_string(rom) + ");";
    }
};

class Laptop : public Product {
    int ram, rom, hasTouchScreen;
public:
    Laptop(string productName, string countryOfOrigin, int price, int ram, int rom, int hasTouchScreen) : Product(productName, countryOfOrigin, price) {
        this->ram = ram;
        this->rom = rom;
        this->hasTouchScreen = hasTouchScreen;
    }

    string to_str() {
        string result = Product::to_str();
        result += "\tRAM: " + to_string(ram) + "\n";
        result += "\tROM: " + to_string(rom) + "\n";
        result += "\tHas touch screen: " + to_string(hasTouchScreen) + "\n";
        return result;
    }

    string getSQLInsertStatement() {
        return "INSERT INTO \"LaptopDetails\" (productName, countryOfOrigin, price, hasTouchScreen, ram, rom) VALUES (\"" + string(productName) + "\",\"" + string(countryOfOrigin) + "\"," + to_string(price) + "," + to_string(hasTouchScreen) + "," + to_string(ram) + "," + to_string(rom) + ");";
    }
};  