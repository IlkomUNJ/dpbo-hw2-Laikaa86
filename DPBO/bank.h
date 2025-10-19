#ifndef BANK_H
#define BANK_H

#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "bank_customer.h"
#include "bank_transaction.h"

class Bank {
   private:
    int id;
    std::string name;
    std::string address;
    std::string phoneNumber;
    std::map<std::string, BankCustomer> customers;
    std::vector<BankTransaction> transactions;

    void loadData();
    void saveData() const;
    int countTodayTransactions(const BankCustomer& customer) const;

   public:
    Bank();
    Bank(int id, std::string name, std::string address, std::string phoneNumber);
    ~Bank();

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getAddress() const { return address; }
    std::string getPhoneNumber() const { return phoneNumber; }

    // Setters
    void setId(int id) { this->id = id; }
    void setName(std::string name) { this->name = name; }
    void setAddress(std::string address) { this->address = address; }
    void setPhoneNumber(std::string phoneNumber) { this->phoneNumber = phoneNumber; }

    // Customer management
    bool addCustomer(const BankCustomer& customer);
    BankCustomer* findCustomer(const std::string& accountNumber);

    // Transaction methods
    bool processTransaction(BankTransaction& transaction);
    std::vector<BankTransaction> getRecentTransactions(int days) const;

    // Analytics methods
    std::vector<BankCustomer> getDormantAccounts() const;
    std::vector<BankCustomer> getMostActiveUsers(int n) const;

    // Utility methods
    std::string generateReport() const;
};

#endif
