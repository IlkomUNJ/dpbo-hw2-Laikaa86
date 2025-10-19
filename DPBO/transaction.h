#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <chrono>
#include <fstream>
#include <string>

#include "buyer.h"
#include "item.h"
#include "seller.h"

enum class TransactionStatus { PENDING, PAID, COMPLETED, CANCELED };

class Transaction {
   private:
    int id;
    int buyerId;
    int sellerId;
    int itemId;
    double amount;
    TransactionStatus status;
    std::chrono::system_clock::time_point timestamp;

   public:
    Transaction()
        : id(0),
          buyerId(0),
          sellerId(0),
          itemId(0),
          amount(0.0),
          status(TransactionStatus::PENDING) {
        timestamp = std::chrono::system_clock::now();
    }

    Transaction(int id, int buyerId, int sellerId, int itemId, double amount)
        : id(id),
          buyerId(buyerId),
          sellerId(sellerId),
          itemId(itemId),
          amount(amount),
          status(TransactionStatus::PENDING) {
        timestamp = std::chrono::system_clock::now();
    }

    // Getters
    int getId() const { return id; }
    int getBuyerId() const { return buyerId; }
    int getSellerId() const { return sellerId; }
    int getItemId() const { return itemId; }
    double getAmount() const { return amount; }
    TransactionStatus getStatus() const { return status; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }

    // Setters
    void setStatus(TransactionStatus status) { this->status = status; }

    // Serialization
    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&id), sizeof(id));
        out.write(reinterpret_cast<const char*>(&buyerId), sizeof(buyerId));
        out.write(reinterpret_cast<const char*>(&sellerId), sizeof(sellerId));
        out.write(reinterpret_cast<const char*>(&itemId), sizeof(itemId));
        out.write(reinterpret_cast<const char*>(&amount), sizeof(amount));
        auto status_val = static_cast<int>(status);
        out.write(reinterpret_cast<const char*>(&status_val), sizeof(status_val));
        auto time = timestamp.time_since_epoch().count();
        out.write(reinterpret_cast<const char*>(&time), sizeof(time));
    }

    void deserialize(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&id), sizeof(id));
        in.read(reinterpret_cast<char*>(&buyerId), sizeof(buyerId));
        in.read(reinterpret_cast<char*>(&sellerId), sizeof(sellerId));
        in.read(reinterpret_cast<char*>(&itemId), sizeof(itemId));
        in.read(reinterpret_cast<char*>(&amount), sizeof(amount));
        int status_val;
        in.read(reinterpret_cast<char*>(&status_val), sizeof(status_val));
        status = static_cast<TransactionStatus>(status_val);
        typename std::chrono::system_clock::duration::rep time;
        in.read(reinterpret_cast<char*>(&time), sizeof(time));
        timestamp =
            std::chrono::system_clock::time_point(std::chrono::system_clock::duration(time));
    }

    bool isWithinDays(int days) const {
        auto now = std::chrono::system_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::hours>(now - timestamp).count();
        return diff <= (days * 24);
    }
};

#endif