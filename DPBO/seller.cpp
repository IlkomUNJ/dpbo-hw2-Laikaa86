#include "seller.h"

#include <algorithm>
#include <chrono>

using std::vector;
using std::chrono::duration_cast;
using std::chrono::hours;
using std::chrono::system_clock;

// Implementation of non-inline methods from seller.h
bool Seller::addItem(const Item& item) {
    auto it = std::find_if(items.begin(), items.end(),
                           [&item](const Item& i) { return i.getId() == item.getId(); });
    if (it == items.end()) {
        items.push_back(item);
        return true;
    }
    return false;
}

bool Seller::deleteItem(int itemId) {
    auto it = std::find_if(items.begin(), items.end(),
                           [itemId](const Item& i) { return i.getId() == itemId; });
    if (it != items.end()) {
        items.erase(it);
        return true;
    }
    return false;
}

bool Seller::restockItem(int itemId, int quantity) {
    auto it = std::find_if(items.begin(), items.end(),
                           [itemId](const Item& i) { return i.getId() == itemId; });
    if (it != items.end()) {
        it->increaseStock(quantity);
        return true;
    }
    return false;
}

bool Seller::setItemPrice(int itemId, double price) {
    auto it = std::find_if(items.begin(), items.end(),
                           [itemId](const Item& i) { return i.getId() == itemId; });
    if (it != items.end()) {
        it->setPrice(price);
        return true;
    }
    return false;
}

std::vector<Item> Seller::getMonthlyPopularItems() const {
    std::vector<Item> monthlyItems = items;
    auto now = std::chrono::system_clock::now();

    // Sort by monthly sales
    std::sort(monthlyItems.begin(), monthlyItems.end(), [this, now](const Item& a, const Item& b) {
        return getMonthlyItemSales(a.getId()) > getMonthlyItemSales(b.getId());
    });

    return monthlyItems;
}

int Seller::getMonthlyItemSales(int itemId) const {
    auto now = std::chrono::system_clock::now();
    return std::count_if(
        transactions.begin(), transactions.end(), [itemId, now](const Transaction& t) {
            auto diff =
                std::chrono::duration_cast<std::chrono::hours>(now - t.getTimestamp()).count();
            return diff <= (30 * 24) && t.getItemId() == itemId &&
                   t.getStatus() == TransactionStatus::COMPLETED;
        });
}
