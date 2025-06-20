#pragma once
#include "cipher.hpp"
#include "entry.hpp"
#include<string>
#include<vector>

struct PasswordFile{
public:
    PasswordFile(const std::string fileName, const std::string ID, const std::string password, const std::string config) : fileName(fileName), ID(ID), password(password), config(config){}

    std::string getName() const{
        return fileName;
    }

    std::string getPassword() const{
        return password;
    }

    void setCipher(Cipher* cipher_ptr_new){
         if (cipher_initialized) {
            throw std::logic_error("Cipher has already been set for this file and cannot be changed.");
        }
        cipher_ptr = cipher_ptr_new;
    }

    Cipher* getCipherPtr() const{
        return cipher_ptr;
    }

    std::string getID() const{
        return ID;
    }

    std::vector<Entry>& getEntries(){
        return entries;
    }

    std::string getConfig() const{
        return config;
    }

private:
    std::string fileName;
    std::string ID;
    std::string password;
    std::string config;
    std::vector<Entry> entries;
    Cipher* cipher_ptr;
    bool cipher_initialized = false;
};