#ifndef SELLER_H
#define SELLER_H

#include <algorithm>
#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "item.h"
#include "transaction.h"
#include "user.h"

class Seller : public User {
   private:
    std::vector<Item> items;
    std::vector<Transaction> transactions;

    struct CustomerStats {
        int purchaseCount;
        double totalAmount;
        std::chrono::system_clock::time_point lastPurchase;
    };

   public:
    Seller() : User() {}

    Seller(int id, std::string name) : User(id, name) {}

    // Item management
    bool addItem(const Item& item) {
        auto it = std::find_if(items.begin(), items.end(),
                               [&item](const Item& i) { return i.getId() == item.getId(); });
        if (it == items.end()) {
            items.push_back(item);
            return true;
        }
        return false;
    }

    bool deleteItem(int itemId) {
        auto it = std::find_if(items.begin(), items.end(),
                               [itemId](const Item& i) { return i.getId() == itemId; });
        if (it != items.end()) {
            items.erase(it);
            return true;
        }
        return false;
    }

    bool restockItem(int itemId, int quantity) {
        auto it = std::find_if(items.begin(), items.end(),
                               [itemId](const Item& i) { return i.getId() == itemId; });
        if (it != items.end()) {
            it->increaseStock(quantity);
            return true;
        }
        return false;
    }

    bool setItemPrice(int itemId, double price) {
        auto it = std::find_if(items.begin(), items.end(),
                               [itemId](const Item& i) { return i.getId() == itemId; });
        if (it != items.end()) {
            it->setPrice(price);
            return true;
        }
        return false;
    }

    // Transaction management
    void addTransaction(const Transaction& transaction) { transactions.push_back(transaction); }

    // Analytics
    std::vector<Item> getMonthlyPopularItems() const {
        std::vector<Item> monthlyItems = items;
        auto now = std::chrono::system_clock::now();

        // Sort by monthly sales
        std::sort(monthlyItems.begin(), monthlyItems.end(),
                  [this, now](const Item& a, const Item& b) {
                      return getMonthlyItemSales(a.getId()) > getMonthlyItemSales(b.getId());
                  });

        return monthlyItems;
    }

    std::vector<int> getLoyalCustomers() const {
        std::map<int, CustomerStats> customerStats;
        auto now = std::chrono::system_clock::now();

        // Calculate customer statistics
        for (const auto& transaction : transactions) {
            if (transaction.getStatus() == TransactionStatus::COMPLETED) {
                auto& stats = customerStats[transaction.getBuyerId()];
                stats.purchaseCount++;
                stats.totalAmount += transaction.getAmount();
                stats.lastPurchase = transaction.getTimestamp();
            }
        }

        // Sort customers by purchase count and total amount
        std::vector<std::pair<int, CustomerStats>> sortedCustomers(customerStats.begin(),
                                                                   customerStats.end());

        std::sort(sortedCustomers.begin(), sortedCustomers.end(), [](const auto& a, const auto& b) {
            if (a.second.purchaseCount == b.second.purchaseCount)
                return a.second.totalAmount > b.second.totalAmount;
            return a.second.purchaseCount > b.second.purchaseCount;
        });

        // Extract customer IDs
        std::vector<int> loyalCustomerIds;
        for (const auto& pair : sortedCustomers) {
            loyalCustomerIds.push_back(pair.first);
        }

        return loyalCustomerIds;
    }

    // Override base class methods
    std::string getInfo() const override {
        std::string info = User::getInfo();
        info += "\nTotal Items: " + std::to_string(items.size());
        info += "\nTotal Transactions: " + std::to_string(transactions.size());
        return info;
    }

   private:
    int getMonthlyItemSales(int itemId) const {
        auto now = std::chrono::system_clock::now();
        return std::count_if(
            transactions.begin(), transactions.end(), [itemId, now](const Transaction& t) {
                auto diff =
                    std::chrono::duration_cast<std::chrono::hours>(now - t.getTimestamp()).count();
                return diff <= (30 * 24) && t.getItemId() == itemId &&
                       t.getStatus() == TransactionStatus::COMPLETED;
            });
    }
};

#endif
