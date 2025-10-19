#ifndef USER_H
#define USER_H

#include <chrono>
#include <string>

class User {
   protected:
    int id;
    std::string name;
    bool loginState;
    std::chrono::system_clock::time_point lastLoginTime;

   public:
    User() : id(0), name(""), loginState(false) {}

    User(int id, std::string name) : id(id), name(name), loginState(false) {}

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    bool isLoggedIn() const { return loginState; }
    std::chrono::system_clock::time_point getLastLoginTime() const { return lastLoginTime; }

    // Setters
    void setId(int id) { this->id = id; }
    void setName(std::string name) { this->name = name; }

    // Login/Logout methods
    virtual bool login(std::string password) {
        loginState = true;
        lastLoginTime = std::chrono::system_clock::now();
        return true;
    }

    virtual void logout() { loginState = false; }

    // Virtual method for getting user info
    virtual std::string getInfo() const { return "ID: " + std::to_string(id) + ", Name: " + name; }

    virtual ~User() = default;
};

#endif