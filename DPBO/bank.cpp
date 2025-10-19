#include "bank.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

void Bank::loadData() {
    std::ifstream file("bank_data.bin", std::ios::binary);
    if (file.is_open()) {
        // Load bank info
        int nameLen, addrLen, phoneLen;
        file.read(reinterpret_cast<char*>(&id), sizeof(id));

        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        char* nameBuf = new char[nameLen + 1];
        file.read(nameBuf, nameLen);
        nameBuf[nameLen] = '\0';
        name = nameBuf;
        delete[] nameBuf;

        // Load customers
        size_t customerCount;
        file.read(reinterpret_cast<char*>(&customerCount), sizeof(customerCount));
        for (size_t i = 0; i < customerCount; i++) {
            BankCustomer customer;
            customer.deserialize(file);
            customers[customer.getAccountNumber()] = customer;
        }

        // Load transactions
        size_t transactionCount;
        file.read(reinterpret_cast<char*>(&transactionCount), sizeof(transactionCount));
        for (size_t i = 0; i < transactionCount; i++) {
            BankTransaction transaction;
            transaction.deserialize(file);
            transactions.push_back(transaction);
        }

        file.close();
    }
}

void Bank::saveData() const {
    std::ofstream file("bank_data.bin", std::ios::binary);
    if (file.is_open()) {
        // Save bank info
        int nameLen = name.length();
        file.write(reinterpret_cast<const char*>(&id), sizeof(id));
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(name.c_str(), nameLen);

        // Save customers
        size_t customerCount = customers.size();
        file.write(reinterpret_cast<const char*>(&customerCount), sizeof(customerCount));
        for (const auto& pair : customers) {
            pair.second.serialize(file);
        }

        // Save transactions
        size_t transactionCount = transactions.size();
        file.write(reinterpret_cast<const char*>(&transactionCount), sizeof(transactionCount));
        for (const auto& transaction : transactions) {
            transaction.serialize(file);
        }

        file.close();
    }
}

// Constructor implementation
Bank::Bank() : id(0), name(""), address(""), phoneNumber("") { loadData(); }

Bank::Bank(int id, std::string name, std::string address, std::string phoneNumber)
    : id(id), name(name), address(address), phoneNumber(phoneNumber) {
    loadData();
}

Bank::~Bank() { saveData(); }

// Customer management implementations
bool Bank::addCustomer(const BankCustomer& customer) {
    std::string accountNumber = customer.getAccountNumber();
    if (customers.find(accountNumber) == customers.end()) {
        customers[accountNumber] = customer;
        return true;
    }
    return false;
}

BankCustomer* Bank::findCustomer(const std::string& accountNumber) {
    auto it = customers.find(accountNumber);
    return it != customers.end() ? &(it->second) : nullptr;
}

// Transaction methods implementation
bool Bank::processTransaction(BankTransaction& transaction) {
    auto* sender = findCustomer(transaction.getFromAccount());
    auto* receiver = findCustomer(transaction.getToAccount());

    if (sender && receiver && sender->getBalance() >= transaction.getAmount()) {
        sender->withdraw(transaction.getAmount());
        receiver->deposit(transaction.getAmount());
        transactions.push_back(transaction);
        return true;
    }
    return false;
}

std::vector<BankTransaction> Bank::getRecentTransactions(int days) const {
    std::vector<BankTransaction> recent;
    auto now = std::chrono::system_clock::now();

    std::copy_if(
        transactions.begin(), transactions.end(), std::back_inserter(recent),
        [now, days](const BankTransaction& t) {
            auto diff =
                std::chrono::duration_cast<std::chrono::hours>(now - t.getTimestamp()).count();
            return diff <= (days * 24);
        });
    return recent;
}

std::vector<BankCustomer> Bank::getDormantAccounts() const {
    std::vector<BankCustomer> dormant;
    auto now = std::chrono::system_clock::now();

    for (const auto& pair : customers) {
        auto lastActivity = pair.second.getLastActivityTime();
        auto diff = std::chrono::duration_cast<std::chrono::hours>(now - lastActivity).count();
        if (diff >= (30 * 24)) {
            dormant.push_back(pair.second);
        }
    }
    return dormant;
}

std::vector<BankCustomer> Bank::getMostActiveUsers(int n) const {
    std::vector<BankCustomer> active;
    for (const auto& pair : customers) {
        active.push_back(pair.second);
    }

    // Sort by transaction count in the last 24 hours
    std::sort(active.begin(), active.end(), [this](const BankCustomer& a, const BankCustomer& b) {
        return countTodayTransactions(a) > countTodayTransactions(b);
    });

    if (active.size() > n) {
        active.resize(n);
    }
    return active;
}

int Bank::countTodayTransactions(const BankCustomer& customer) const {
    auto now = std::chrono::system_clock::now();
    return std::count_if(
        transactions.begin(), transactions.end(), [&customer, now](const BankTransaction& t) {
            auto diff =
                std::chrono::duration_cast<std::chrono::hours>(now - t.getTimestamp()).count();
            return diff <= 24 && (t.getFromAccount() == customer.getAccountNumber() ||
                                  t.getToAccount() == customer.getAccountNumber());
        });
}

std::string Bank::generateReport() const {
    std::ostringstream report;
    auto now = std::chrono::system_clock::now();

    report << "Bank Report - " << name << "\n";
    report << "================================\n\n";

    // Customer Statistics
    report << "Customer Statistics:\n";
    report << "Total Customers: " << customers.size() << "\n";
    report << "Dormant Accounts: " << getDormantAccounts().size() << "\n\n";

    // Transaction Statistics
    auto recentTrans = getRecentTransactions(7);
    report << "Transaction Statistics (Last 7 Days):\n";
    report << "Total Transactions: " << recentTrans.size() << "\n";

    double totalValue = 0.0;
    for (const auto& trans : recentTrans) {
        totalValue += trans.getAmount();
    }
    report << "Total Transaction Value: $" << std::fixed << std::setprecision(2) << totalValue
           << "\n\n";

    // Most Active Users
    auto activeUsers = getMostActiveUsers(5);
    report << "Top 5 Most Active Users Today:\n";
    for (const auto& user : activeUsers) {
        report << user.getName() << " - " << countTodayTransactions(user) << " transactions\n";
    }

    return report.str();
}
