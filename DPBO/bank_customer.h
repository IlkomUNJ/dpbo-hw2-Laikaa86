#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <chrono>
#include <fstream>
#include <string>
#include <vector>

#include "bank_transaction.h"

class BankCustomer {
   private:
    int id;
    std::string name;
    std::string accountNumber;
    double balance;
    std::chrono::system_clock::time_point lastActivityTime;
    std::vector<BankTransaction> transactions;

   public:
    BankCustomer() : id(0), name(""), accountNumber(""), balance(0.0) {
        lastActivityTime = std::chrono::system_clock::now();
    }

    BankCustomer(int id, std::string name, std::string accountNumber)
        : id(id), name(name), accountNumber(accountNumber), balance(0.0) {
        lastActivityTime = std::chrono::system_clock::now();
    }

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    std::chrono::system_clock::time_point getLastActivityTime() const { return lastActivityTime; }
    const std::vector<BankTransaction>& getTransactions() const { return transactions; }

    // Transaction methods
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            lastActivityTime = std::chrono::system_clock::now();
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            lastActivityTime = std::chrono::system_clock::now();
            return true;
        }
        return false;
    }

    void addTransaction(const BankTransaction& transaction) {
        transactions.push_back(transaction);
        lastActivityTime = std::chrono::system_clock::now();
    }

    // Serialization
    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&id), sizeof(id));

        int nameLen = name.length();
        out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        out.write(name.c_str(), nameLen);

        int accLen = accountNumber.length();
        out.write(reinterpret_cast<const char*>(&accLen), sizeof(accLen));
        out.write(accountNumber.c_str(), accLen);

        out.write(reinterpret_cast<const char*>(&balance), sizeof(balance));

        auto time = lastActivityTime.time_since_epoch().count();
        out.write(reinterpret_cast<const char*>(&time), sizeof(time));

        size_t transCount = transactions.size();
        out.write(reinterpret_cast<const char*>(&transCount), sizeof(transCount));
        for (const auto& trans : transactions) {
            trans.serialize(out);
        }
    }

    void deserialize(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&id), sizeof(id));

        int nameLen;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        char* nameBuf = new char[nameLen + 1];
        in.read(nameBuf, nameLen);
        nameBuf[nameLen] = '\0';
        name = nameBuf;
        delete[] nameBuf;

        int accLen;
        in.read(reinterpret_cast<char*>(&accLen), sizeof(accLen));
        char* accBuf = new char[accLen + 1];
        in.read(accBuf, accLen);
        accBuf[accLen] = '\0';
        accountNumber = accBuf;
        delete[] accBuf;

        in.read(reinterpret_cast<char*>(&balance), sizeof(balance));

        typename std::chrono::system_clock::duration::rep time;
        in.read(reinterpret_cast<char*>(&time), sizeof(time));
        lastActivityTime =
            std::chrono::system_clock::time_point(std::chrono::system_clock::duration(time));

        size_t transCount;
        in.read(reinterpret_cast<char*>(&transCount), sizeof(transCount));
        transactions.clear();
        for (size_t i = 0; i < transCount; i++) {
            BankTransaction trans;
            trans.deserialize(in);
            transactions.push_back(trans);
        }
    }
};

#endif