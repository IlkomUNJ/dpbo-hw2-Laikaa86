#ifndef STORE_H
#define STORE_H

#include <algorithm>
#include <chrono>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "buyer.h"
#include "item.h"
#include "seller.h"
#include "transaction.h"

class Store {
   private:
    std::vector<Transaction> transactions;
    std::map<int, Item> items;
    std::map<int, Buyer> buyers;
    std::map<int, Seller> sellers;

    void loadData() {
        std::ifstream file("store_data.bin", std::ios::binary);
        if (file.is_open()) {
            // Load items
            size_t itemCount;
            file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
            for (size_t i = 0; i < itemCount; i++) {
                Item item;
                item.deserialize(file);
                items[item.getId()] = item;
            }

            // Load transactions
            size_t transactionCount;
            file.read(reinterpret_cast<char*>(&transactionCount), sizeof(transactionCount));
            for (size_t i = 0; i < transactionCount; i++) {
                Transaction transaction;
                transaction.deserialize(file);
                transactions.push_back(transaction);
            }

            file.close();
        }
    }

    void saveData() const {
        std::ofstream file("store_data.bin", std::ios::binary);
        if (file.is_open()) {
            // Save items
            size_t itemCount = items.size();
            file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
            for (const auto& pair : items) {
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

   public:
    Store() { loadData(); }

    ~Store() { saveData(); }

    // Transaction management
    std::vector<Transaction> getTransactionsInLastDays(int days) const {
        std::vector<Transaction> recent;
        auto now = std::chrono::system_clock::now();

        std::copy_if(transactions.begin(), transactions.end(), std::back_inserter(recent),
                     [days](const Transaction& t) { return t.isWithinDays(days); });
        return recent;
    }

    std::vector<Transaction> getPendingTransactions() const {
        std::vector<Transaction> pending;
        std::copy_if(transactions.begin(), transactions.end(), std::back_inserter(pending),
                     [](const Transaction& t) { return t.getStatus() == TransactionStatus::PAID; });
        return pending;
    }

    std::vector<Item> getMostSoldItems(int count) const {
        std::vector<Item> sortedItems;
        for (const auto& pair : items) {
            sortedItems.push_back(pair.second);
        }

        std::sort(sortedItems.begin(), sortedItems.end(),
                  [](const Item& a, const Item& b) { return a.getSoldCount() > b.getSoldCount(); });

        if (sortedItems.size() > count) {
            sortedItems.resize(count);
        }
        return sortedItems;
    }

    std::pair<Buyer*, Seller*> getMostActiveUsersToday() {
        Buyer* topBuyer = nullptr;
        Seller* topSeller = nullptr;
        int maxBuyerCount = 0;
        int maxSellerCount = 0;

        auto now = std::chrono::system_clock::now();

        for (auto& pair : buyers) {
            int count = countTodayTransactions(pair.second.getId(), true);
            if (count > maxBuyerCount) {
                maxBuyerCount = count;
                topBuyer = &pair.second;
            }
        }

        for (auto& pair : sellers) {
            int count = countTodayTransactions(pair.second.getId(), false);
            if (count > maxSellerCount) {
                maxSellerCount = count;
                topSeller = &pair.second;
            }
        }

        return {topBuyer, topSeller};
    }

    bool processTransaction(Transaction& transaction) {
        if (transaction.getStatus() == TransactionStatus::PENDING) {
            Item* item = findItem(transaction.getItemId());
            if (item && item->decreaseStock(1)) {
                transactions.push_back(transaction);
                return true;
            }
        }
        return false;
    }

    // Item management
    Item* findItem(int itemId) {
        auto it = items.find(itemId);
        return it != items.end() ? &(it->second) : nullptr;
    }

    bool addItem(const Item& item) {
        if (items.find(item.getId()) == items.end()) {
            items[item.getId()] = item;
            return true;
        }
        return false;
    }

   private:
    int countTodayTransactions(int userId, bool isBuyer) const {
        auto now = std::chrono::system_clock::now();
        return std::count_if(
            transactions.begin(), transactions.end(), [userId, isBuyer, now](const Transaction& t) {
                auto diff =
                    std::chrono::duration_cast<std::chrono::hours>(now - t.getTimestamp()).count();
                return diff <= 24 &&
                       (isBuyer ? t.getBuyerId() == userId : t.getSellerId() == userId);
            });
    }
};

#endif