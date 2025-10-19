#ifndef ITEM_H
#define ITEM_H

#include <chrono>
#include <fstream>
#include <string>

class Item {
   private:
    int id;
    std::string name;
    double price;
    int stock;
    int soldCount;
    std::chrono::system_clock::time_point lastRestockTime;

   public:
    Item() : id(0), name(""), price(0.0), stock(0), soldCount(0) {
        lastRestockTime = std::chrono::system_clock::now();
    }

    Item(int id, std::string name, double price, int stock)
        : id(id), name(name), price(price), stock(stock), soldCount(0) {
        lastRestockTime = std::chrono::system_clock::now();
    }

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    int getSoldCount() const { return soldCount; }
    std::chrono::system_clock::time_point getLastRestockTime() const { return lastRestockTime; }

    // Setters
    void setId(int id) { this->id = id; }
    void setName(std::string name) { this->name = name; }
    void setPrice(double price) { this->price = price; }
    void setStock(int stock) {
        this->stock = stock;
        this->lastRestockTime = std::chrono::system_clock::now();
    }

    // Business logic
    bool decreaseStock(int amount) {
        if (stock >= amount) {
            stock -= amount;
            soldCount += amount;
            return true;
        }
        return false;
    }

    void increaseStock(int amount) {
        stock += amount;
        lastRestockTime = std::chrono::system_clock::now();
    }

    // Serialization
    void serialize(std::ofstream& out) const {
        int nameLength = name.length();
        out.write(reinterpret_cast<const char*>(&id), sizeof(id));
        out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        out.write(name.c_str(), nameLength);
        out.write(reinterpret_cast<const char*>(&price), sizeof(price));
        out.write(reinterpret_cast<const char*>(&stock), sizeof(stock));
        out.write(reinterpret_cast<const char*>(&soldCount), sizeof(soldCount));
        auto time = lastRestockTime.time_since_epoch().count();
        out.write(reinterpret_cast<const char*>(&time), sizeof(time));
    }

    void deserialize(std::ifstream& in) {
        int nameLength;
        in.read(reinterpret_cast<char*>(&id), sizeof(id));
        in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        char* nameBuffer = new char[nameLength + 1];
        in.read(nameBuffer, nameLength);
        nameBuffer[nameLength] = '\0';
        name = std::string(nameBuffer);
        delete[] nameBuffer;
        in.read(reinterpret_cast<char*>(&price), sizeof(price));
        in.read(reinterpret_cast<char*>(&stock), sizeof(stock));
        in.read(reinterpret_cast<char*>(&soldCount), sizeof(soldCount));
        typename std::chrono::system_clock::duration::rep time;
        in.read(reinterpret_cast<char*>(&time), sizeof(time));
        lastRestockTime =
            std::chrono::system_clock::time_point(std::chrono::system_clock::duration(time));
    }
};

#endif