#ifndef BUYER_H
#define BUYER_H

#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "bank_customer.h"
#include "transaction.h"
#include "user.h"

class Buyer : public User {
   private:
    BankCustomer* bankAccount;
    std::vector<Transaction> transactions;

   public:
    Buyer() : User(), bankAccount(nullptr) {}

    Buyer(int id, std::string name) : User(id, name), bankAccount(nullptr) {}

    // Bank account management
    void createBankAccount(BankCustomer* account) { this->bankAccount = account; }

    bool hasBankAccount() const { return bankAccount != nullptr; }

    // Money management
    bool deposit(double amount) {
        if (bankAccount && amount > 0) {
            bankAccount->deposit(amount);
            return true;
        }
        return false;
    }

    bool withdraw(double amount) {
        if (bankAccount && amount > 0 && bankAccount->getBalance() >= amount) {
            bankAccount->withdraw(amount);
            return true;
        }
        return false;
    }

    double getBalance() const { return bankAccount ? bankAccount->getBalance() : 0.0; }

    // Transaction management
    void addTransaction(const Transaction& transaction) { transactions.push_back(transaction); }

    std::vector<Transaction> getTransactions(TransactionStatus status) const {
        std::vector<Transaction> filtered;
        std::copy_if(transactions.begin(), transactions.end(), std::back_inserter(filtered),
                     [status](const Transaction& t) { return t.getStatus() == status; });
        return filtered;
    }

    double getTotalSpending(int days) const {
        double total = 0.0;
        auto now = std::chrono::system_clock::now();

        for (const auto& transaction : transactions) {
            if (transaction.isWithinDays(days) &&
                transaction.getStatus() != TransactionStatus::CANCELED) {
                total += transaction.getAmount();
            }
        }
        return total;
    }

    std::vector<std::pair<std::chrono::system_clock::time_point, double>> getCashFlow(
        bool monthly = false) const {
        std::vector<std::pair<std::chrono::system_clock::time_point, double>> flow;

        // Group transactions by day or month
        std::map<std::chrono::system_clock::time_point, double> groupedFlow;

        for (const auto& transaction : transactions) {
            auto time = transaction.getTimestamp();
            if (monthly) {
                // Truncate to first day of month
                auto tm = std::chrono::system_clock::to_time_t(time);
                std::tm* ltm = std::localtime(&tm);
                ltm->tm_mday = 1;
                time = std::chrono::system_clock::from_time_t(std::mktime(ltm));
            }
            groupedFlow[time] += transaction.getAmount();
        }

        // Convert map to vector
        for (const auto& pair : groupedFlow) {
            flow.push_back(pair);
        }

        return flow;
    }

    // Override base class methods
    std::string getInfo() const override {
        std::string info = User::getInfo();
        if (bankAccount) {
            info += "\nBalance: " + std::to_string(bankAccount->getBalance());
        }
        return info;
    }
};

#endif
