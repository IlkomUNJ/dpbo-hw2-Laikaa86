#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "bank.h"
#include "buyer.h"
#include "item.h"
#include "seller.h"
#include "store.h"
#include "transaction.h"
#include "user.h"

// Utility functions
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void displayHeader(const std::string& title) {
    clearScreen();
    std::cout << std::string(50, '=') << "\n";
    std::cout << std::setw(25 + title.length() / 2) << title << "\n";
    std::cout << std::string(50, '=') << "\n\n";
}

class ECommerceSystem {
   private:
    Bank bank;
    Store store;
    User* currentUser;

    void showBankMenu() {
        int choice;
        do {
            displayHeader("Bank Management System");
            std::cout << "1. Show Recent Transactions (7 days)\n";
            std::cout << "2. Show Dormant Accounts\n";
            std::cout << "3. Show Most Active Users\n";
            std::cout << "4. Generate Bank Report\n";
            std::cout << "5. Add New Customer\n";
            std::cout << "0. Back to Main Menu\n\n";
            std::cout << "Choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    auto transactions = bank.getRecentTransactions(7);
                    displayHeader("Recent Transactions");
                    for (const auto& trans : transactions) {
                        std::cout << "From: " << trans.getFromAccount()
                                  << " To: " << trans.getToAccount() << " Amount: $"
                                  << trans.getAmount() << "\n";
                    }
                    break;
                }
                case 2: {
                    auto dormant = bank.getDormantAccounts();
                    displayHeader("Dormant Accounts");
                    for (const auto& acc : dormant) {
                        std::cout << "Account: " << acc.getAccountNumber()
                                  << " Name: " << acc.getName() << "\n";
                    }
                    break;
                }
                case 3: {
                    auto active = bank.getMostActiveUsers(5);
                    displayHeader("Most Active Users");
                    for (const auto& user : active) {
                        std::cout << "Name: " << user.getName()
                                  << " Account: " << user.getAccountNumber() << "\n";
                    }
                    break;
                }
                case 4: {
                    displayHeader("Bank Report");
                    std::cout << bank.generateReport();
                    break;
                }
                case 5: {
                    std::string name, accountNum;
                    std::cout << "Enter customer name: ";
                    std::cin.ignore();
                    std::getline(std::cin, name);
                    std::cout << "Enter account number: ";
                    std::cin >> accountNum;

                    BankCustomer newCustomer(bank.getId(), name, accountNum);
                    if (bank.addCustomer(newCustomer)) {
                        std::cout << "Customer added successfully!\n";
                    } else {
                        std::cout << "Account number already exists!\n";
                    }
                    break;
                }
            }
            if (choice != 0) pauseScreen();
        } while (choice != 0);
    }

    void showStoreMenu() {
        int choice;
        do {
            displayHeader("Store Management System");
            std::cout << "1. Show Recent Store Transactions\n";
            std::cout << "2. Show Pending Transactions\n";
            std::cout << "3. Show Most Sold Items\n";
            std::cout << "4. Show Most Active Users Today\n";
            std::cout << "5. Add New Item\n";
            std::cout << "0. Back to Main Menu\n\n";
            std::cout << "Choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    int days;
                    std::cout << "Enter number of days: ";
                    std::cin >> days;
                    auto transactions = store.getTransactionsInLastDays(days);
                    displayHeader("Recent Store Transactions");
                    for (const auto& trans : transactions) {
                        std::cout << "Buyer: " << trans.getBuyerId()
                                  << " Item: " << trans.getItemId() << " Amount: $"
                                  << trans.getAmount() << "\n";
                    }
                    break;
                }
                case 2: {
                    auto pending = store.getPendingTransactions();
                    displayHeader("Pending Transactions");
                    for (const auto& trans : pending) {
                        std::cout << "Transaction ID: " << trans.getId() << " Amount: $"
                                  << trans.getAmount() << "\n";
                    }
                    break;
                }
                case 3: {
                    auto popular = store.getMostSoldItems(5);
                    displayHeader("Most Sold Items");
                    for (const auto& item : popular) {
                        std::cout << "Item: " << item.getName()
                                  << " Sold Count: " << item.getSoldCount() << "\n";
                    }
                    break;
                }
                case 4: {
                    auto [topBuyer, topSeller] = store.getMostActiveUsersToday();
                    displayHeader("Most Active Users Today");
                    if (topBuyer) std::cout << "Top Buyer: " << topBuyer->getName() << "\n";
                    if (topSeller) std::cout << "Top Seller: " << topSeller->getName() << "\n";
                    break;
                }
                case 5: {
                    std::string name;
                    double price;
                    int stock;

                    std::cout << "Enter item name: ";
                    std::cin.ignore();
                    std::getline(std::cin, name);
                    std::cout << "Enter price: ";
                    std::cin >> price;
                    std::cout << "Enter initial stock: ";
                    std::cin >> stock;

                    Item newItem(store.getMostSoldItems(1).size() + 1, name, price, stock);
                    if (store.addItem(newItem)) {
                        std::cout << "Item added successfully!\n";
                    } else {
                        std::cout << "Item ID already exists!\n";
                    }
                    break;
                }
            }
            if (choice != 0) pauseScreen();
        } while (choice != 0);
    }

   public:
    ECommerceSystem() : currentUser(nullptr) {
        // Initialize with some data
        bank = Bank(1, "E-Commerce Bank", "Digital Street 123", "123-456-789");
        store = Store();
    }

    void run() {
        int choice;
        do {
            displayHeader("E-Commerce System");
            std::cout << "1. Bank Management\n";
            std::cout << "2. Store Management\n";
            std::cout << "3. User Management\n";
            std::cout << "0. Exit\n\n";
            std::cout << "Choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1:
                    showBankMenu();
                    break;
                case 2:
                    showStoreMenu();
                    break;
                case 3:
                    // User management menu would go here
                    break;
                case 0:
                    std::cout << "\nSaving all data...\n";
                    break;
                default:
                    std::cout << "\nInvalid choice!\n";
                    pauseScreen();
            }
        } while (choice != 0);
    }
};

int main() {
    try {
        ECommerceSystem system;
        system.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
