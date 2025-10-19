#ifndef BANK_TRANSACTION_H
#define BANK_TRANSACTION_H

#include <chrono>
#include <fstream>
#include <string>

class BankTransaction {
   private:
    int id;
    std::string fromAccount;
    std::string toAccount;
    double amount;
    std::chrono::system_clock::time_point timestamp;
    std::string description;

   public:
    BankTransaction() : id(0), fromAccount(""), toAccount(""), amount(0.0) {
        timestamp = std::chrono::system_clock::now();
    }

    BankTransaction(int id, std::string from, std::string to, double amount, std::string desc)
        : id(id), fromAccount(from), toAccount(to), amount(amount), description(desc) {
        timestamp = std::chrono::system_clock::now();
    }

    // Getters
    int getId() const { return id; }
    std::string getFromAccount() const { return fromAccount; }
    std::string getToAccount() const { return toAccount; }
    double getAmount() const { return amount; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    std::string getDescription() const { return description; }

    // Serialization
    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&id), sizeof(id));

        int fromLen = fromAccount.length();
        out.write(reinterpret_cast<const char*>(&fromLen), sizeof(fromLen));
        out.write(fromAccount.c_str(), fromLen);

        int toLen = toAccount.length();
        out.write(reinterpret_cast<const char*>(&toLen), sizeof(toLen));
        out.write(toAccount.c_str(), toLen);

        out.write(reinterpret_cast<const char*>(&amount), sizeof(amount));

        auto time = timestamp.time_since_epoch().count();
        out.write(reinterpret_cast<const char*>(&time), sizeof(time));

        int descLen = description.length();
        out.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
        out.write(description.c_str(), descLen);
    }

    void deserialize(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&id), sizeof(id));

        int fromLen;
        in.read(reinterpret_cast<char*>(&fromLen), sizeof(fromLen));
        char* fromBuf = new char[fromLen + 1];
        in.read(fromBuf, fromLen);
        fromBuf[fromLen] = '\0';
        fromAccount = fromBuf;
        delete[] fromBuf;

        int toLen;
        in.read(reinterpret_cast<char*>(&toLen), sizeof(toLen));
        char* toBuf = new char[toLen + 1];
        in.read(toBuf, toLen);
        toBuf[toLen] = '\0';
        toAccount = toBuf;
        delete[] toBuf;

        in.read(reinterpret_cast<char*>(&amount), sizeof(amount));

        typename std::chrono::system_clock::duration::rep time;
        in.read(reinterpret_cast<char*>(&time), sizeof(time));
        timestamp =
            std::chrono::system_clock::time_point(std::chrono::system_clock::duration(time));

        int descLen;
        in.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
        char* descBuf = new char[descLen + 1];
        in.read(descBuf, descLen);
        descBuf[descLen] = '\0';
        description = descBuf;
        delete[] descBuf;
    }
};

#endif