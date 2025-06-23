#pragma once
#include "passwordFile.hpp"
#include<string>
#include<vector>
#include<iostream>

class PasswordManager {
public:

    void create(const std::string& fileName, const std::string& cipherType,const std::string& password, const std::string& cipherArgs);
    void open(const std::string& fileName, const std::string& password);
    void save(const std::string& website, const std::string& user, const std::string& pass);
    void load(const std::string& website, const std::string& user = "");
    void update(const std::string& website, const std::string& user, const std::string& newPass);
    void del(const std::string& website, const std::string& user = "");

    static PasswordManager& getInstance() {
        static PasswordManager instance;
        return instance;
    }

    ~PasswordManager(){
        size_t size = files.size();
        for(size_t i=0;i<size;i++){
            delete files[i];
        }
    }

private:
    PasswordManager() {}
    PasswordManager(const PasswordManager&) = delete;
    PasswordManager& operator=(const PasswordManager&) = delete;

    bool nameTaken(const std::string& name);

private:

    int current_open_idx = -1;
    std::vector<PasswordFile*> files;
};