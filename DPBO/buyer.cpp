#include "buyer.h"

#include <algorithm>
#include <chrono>

using std::vector;
using std::chrono::duration_cast;
using std::chrono::hours;
using std::chrono::system_clock;

// Implementation of non-inline methods from buyer.h
void Buyer::createBankAccount(BankCustomer* account) { this->bankAccount = account; }

bool Buyer::deposit(double amount) {
    if (bankAccount && amount > 0) {
        bankAccount->deposit(amount);
        return true;
    }
    return false;
}

bool Buyer::withdraw(double amount) {
    if (bankAccount && amount > 0 && bankAccount->getBalance() >= amount) {
        bankAccount->withdraw(amount);
        return true;
    }
    return false;
}

void Buyer::addTransaction(const Transaction& transaction) { transactions.push_back(transaction); }

std::vector<Transaction> Buyer::getTransactions(TransactionStatus status) const {
    std::vector<Transaction> filtered;
    std::copy_if(transactions.begin(), transactions.end(), std::back_inserter(filtered),
                 [status](const Transaction& t) { return t.getStatus() == status; });
    return filtered;
}

double Buyer::getTotalSpending(int days) const {
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
