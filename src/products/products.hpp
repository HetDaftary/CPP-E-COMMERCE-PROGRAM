#include <iostream>
#include "nlohmann/json.hpp"

using std::string;
using std::ostream;
using nlohmann::json;
using std::to_string;

class Product {
protected:
    json data;
public:
    Product(string productName, string countryOfOrigin, int price) {
        data["productName"] = productName;
        data["countryOfOrigin"] = countryOfOrigin;
        data["price"] = price;
    }

    string to_str() {
        return data.dump(4);
    }

    string getProductName() {
        return data["productName"];
    }

    // This method needs to be defined in every derived class.
    // Concept of interface is used here.
    virtual string getSQLInsertStatement() = 0;
};

class Smartphone : public Product {
public:
    Smartphone(string productName, string countryOfOrigin, int price, int numberOfCameras, string processor, int ram, int rom) : Product(productName, countryOfOrigin, price) {
        data["numberOfCameras"] = numberOfCameras;
        data["processor"] = processor;
        data["ram"] = ram;
        data["rom"] = rom;
    }

    string getSQLInsertStatement() {
        return "INSERT INTO \"SmartphoneDetails\" (productName, countryOfOrigin, price, numberOfCameras, processor, ram, rom) VALUES (\""+ string(data["productName"]) + "\",\"" + string(data["countryOfOrigin"]) + string("\",\"") + to_string(data["price"]) + "\",\"" + to_string(data["numberOfCameras"]) + "\",\"" + string(data["processor"]) + "\",\"" + to_string(data["ram"]) + "\",\"" + to_string(data["rom"]) + "\");";
    }
};

class Laptop : public Product {
public:
    Laptop(string productName, string countryOfOrigin, int price, int ram, int rom, int hasTouchScreen) : Product(productName, countryOfOrigin, price) {
        data["ram"] = ram;
        data["rom"] = rom;
        data["hasTouchScreen"] = hasTouchScreen;
    }

    string getSQLInsertStatement() {
        return "INSERT INTO \"LaptopDetails\" (productName, countryOfOrigin, price, hasTouchScreen, ram, rom) VALUES (\"" + string(data["productName"]) + "\",\"" + string(data["countryOfOrigin"]) + "\",\"" + to_string(data["price"]) + "\",\"" + to_string(data["hasTouchScreen"]) + "\",\"" + to_string(data["ram"]) + "\",\"" + to_string(data["rom"]) + "\");";
    }
};  