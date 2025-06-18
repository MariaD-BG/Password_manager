#include "passwordManager.hpp"
#include "passwordFile.hpp"

#include<fstream>
#include<iostream>

void PasswordManager::create(const std::string& fileName, const std::string& cipherType, const std::string& password, const std::string& cipherArgs) {
    if (nameTaken(fileName)) {
        throw std::runtime_error("File with this name already exists.");
    }

    std::ofstream fileOut(fileName); 
    if (!fileOut) {
        throw std::runtime_error("Failed to create file: " + fileName);
    }

    fileOut << "CIPHER " << cipherType << "\n";
    fileOut << "PASSWORD " << password << "\n"; 
    fileOut << "ARGS " << cipherArgs << "\n";

    // TODO: validate arguments wrt to cipher type 

    fileOut.close();

    PasswordFile newFile(fileName, cipherType, password, cipherArgs);
    files.push_back(std::move(newFile));

    std::cout << "File '" << fileName << "' created successfully.\n";
}

void PasswordManager::open(const std::string& fileName, const std::string& password){
    size_t numFiles = files.size();
    std::cout<<"There are "<<numFiles<<" files in collection\n";
    for(size_t i=0;i<numFiles;i++){
        if(files[i].getName() == fileName){
            if(files[i].getPassword() == password){
                current_open = &files[i];
                std::cout<<"File opened successfully!\n";
                return;
            }else{
                throw std::invalid_argument("Invalid password -- you have no permission to open this file!\n");
                return;
            }
        }
    }
    // std::cerr<<"File not found! Command unsuccessful!\n";
    throw std::runtime_error("File not found! Command unsuccessful!");
}

void PasswordManager::save(const std::string& website, const std::string& user, const std::string& pass) {
    // TODO: Implement this
}

void PasswordManager::load(const std::string& website, const std::string& user) {
    // TODO: Implement this
}

void PasswordManager::update(const std::string& website, const std::string& user, const std::string& newPass) {
    // TODO: Implement this
}

void PasswordManager::del(const std::string& website, const std::string& user) {
    // TODO: Implement this
}

bool PasswordManager::nameTaken(const std::string& name){
    size_t numFiles = files.size();
    for(size_t i=0;i<numFiles;i++){
        if(files[i].getName() == name){
            return true;
        }
    }
    return false;
}