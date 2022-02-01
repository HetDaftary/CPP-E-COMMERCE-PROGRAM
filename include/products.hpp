#ifndef _PRODUCTS_HPP
#define _PRODUCTS_HPP

#include <iostream>
#include <cstdio>

#define bufferSize 512

using std::string;
using std::ostream;
using std::to_string;

/**
 * @brief The Product class is the base class for all the products.
 * 
 */
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
    /**
     * @brief Construct a new Product object
     * 
     * @param productName 
     * @param countryOfOrigin 
     * @param price 
     * @param stock 
     * @param type 
     */
    Product(string productName, string countryOfOrigin, int price, int stock, ProductType type);
    Product(char* productName, char* countryOfOrigin, int price, int stock, ProductType type);

    /**
     * @brief Converts the product info to a string.
     * 
     * @return string 
     */
    string toStr();

    /**
     * @brief Get the Product Name.
     * 
     * @return string 
     */
    string getProductName();

    /**
     * @brief This returns the sqlite insert query.
     * 
     * @return char* 
     */
    //! This method needs to be defined in every derived class.
    //! Concept of interface is used here.
    virtual char* getSQLInsertStatement() = 0;
};

class Smartphone : public Product {
    int ram, rom, numberOfCameras;
    string processor;
public:
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
    Smartphone(string productName, string countryOfOrigin, int price, int stock, int numberOfCameras, string processor, int ram, int rom); 
    Smartphone(char* productName, char* countryOfOrigin, int price, int stock, int numberOfCameras, char* processor, int ram, int rom);

    /**
     * @brief Converts the smartphone info to a string.
     * 
     * @return string 
     */
    string toStr();

    /**
     * @brief Returns the INSERT statement for the smartphone.
     * 
     * @return char* 
     */
    char* getSQLInsertStatement();
};

class Laptop : public Product {
    int ram, rom, hasTouchScreen;
public:
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
    Laptop(string productName, string countryOfOrigin, int price, int stock, int ram, int rom, int hasTouchScreen);
    Laptop(char* productName, char* countryOfOrigin, int price, int stock, int ram, int rom, int hasTouchScreen);

    /**
     * @brief Converts the laptop info to a string.
     * 
     * @return string 
     */
    string toStr();

    /**
     * @brief Returns the INSERT statement for the laptop.
     * 
     * @return char* 
     */
    char* getSQLInsertStatement();
};  
#endif