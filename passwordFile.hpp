#pragma once
#include<string>

struct PasswordFile{
public:
    PasswordFile(const std::string fileName, const std::string ID, const std::string password, const std::string config) : fileName(fileName), ID(ID), password(password), config(config){}

    std::string getName() const{
        return fileName;
    }

    std::string getPassword() const{
        return password;
    }
private:
    std::string fileName;
    std::string ID;
    std::string password;
    std::string config;
};