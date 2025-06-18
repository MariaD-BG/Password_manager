#include "passwordManager.hpp"
#include "passwordFile.hpp"
#include "caesarCipher.hpp"
#include "textCodeCipher.hpp"
#include "hillCipher.hpp"
#include "utils.hpp"

#include<fstream>
#include<iostream>

void PasswordManager::create(const std::string& fileName, const std::string& cipherType, const std::string& password, const std::string& cipherArgs) {

    std::string fileNameNew = Utils::fixTxtExtension(fileName);

    if (nameTaken(fileNameNew)) {
        throw std::runtime_error("File with this name already exists.");
    }

    bool validity;

    if (cipherType == "CAESAR") {
        try{
            validity = CaesarCipher::validateConfig(cipherArgs);
        }catch(const std::exception& e){
            throw std::invalid_argument("Config not valid for cipher type " + cipherType + ":" + e.what());
        }
        if(!validity) throw std::invalid_argument("Config not valid for cipher type " + cipherType);
    } else if (cipherType == "TEXTCODE") {
        try{
            validity = TextCodeCipher::validateConfig(cipherArgs);
        }catch(const std::exception& e){
            throw std::invalid_argument("Config not valid for cipher type " + cipherType + ":" + e.what());
        }
        if(!validity) throw std::invalid_argument("Config not valid for cipher type " + cipherType);
    } else if (cipherType == "HILL") {
        try{
            validity = HillCipher::validateConfig(cipherArgs);
        }catch(const std::exception& e){
            throw std::invalid_argument("Config not valid for cipher type " + cipherType + ":" + e.what());
        }
        if(!validity) throw std::invalid_argument("Config not valid for cipher type " + cipherType);
    } else {
        throw std::invalid_argument("Unknown cipher type: " + cipherType);
    }
    

    std::ofstream fileOut(fileNameNew); 
    if (!fileOut) {
        throw std::runtime_error("Failed to create file: " + fileNameNew);
    }
    fileOut << "CIPHER " << cipherType << "\n";
    fileOut << "PASSWORD " << password << "\n"; 
    fileOut << "ARGS " << cipherArgs << "\n";

    fileOut.close();

    PasswordFile newFile(fileNameNew, cipherType, password, cipherArgs);
    // newFile.setCipher();

    if (cipherType == "CAESAR") {
        newFile.setCipher(CaesarCipher::createCipherFromConfig(cipherArgs));
    } else if (cipherType == "TEXTCODE") {
        newFile.setCipher(TextCodeCipher::createCipherFromConfig(cipherArgs));
    } else if (cipherType == "HILL") {
        newFile.setCipher(HillCipher::createCipherFromConfig(cipherArgs));
    } 

    files.push_back(std::move(newFile));

    std::cout << "File '" << fileNameNew << "' created successfully.\n";
}

void PasswordManager::open(const std::string& fileName, const std::string& password){
    size_t numFiles = files.size();
    std::string fileNameNew = Utils::fixTxtExtension(fileName);
    std::cout<<"There are "<<numFiles<<" files in collection\n";
    for(size_t i=0;i<numFiles;i++){
        if(files[i].getName() == fileNameNew){
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
    
    if (!current_open) throw std::runtime_error("No password file is open.");
    
    std::string encodedPass;
    try {
        EncryptedMessage encoded = current_open->getCipherPtr()->encrypt(pass);
        if(current_open->getID() == "CAESAR") encodedPass = encoded.serialize(true); 
        else encodedPass = encoded.serialize(false);
    } catch(...) {
        throw std::runtime_error("Failed to encrypt password!");
    }

    // 2. Update entries vector
    std::vector<Entry>& entries = current_open->getEntries();
    bool updated = false;
    for (Entry& entry : entries) {
        if (entry.website == website && entry.user == user) {
            entry.encodedPass = encodedPass;
            updated = true;
            break;
        }
    }
    if (!updated) {
        entries.push_back({website, user, encodedPass});
    }

    // 3. Write password file to disk (header + entries)
    std::ofstream out(current_open->getName());
    if (!out) throw std::runtime_error("Failed to open password file for writing.");

    out << "CIPHER " << current_open->getID() << "\n";
    out << "PASSWORD " << current_open->getPassword() << "\n";
    out << "ARGS " << current_open->getConfig() << "\n";
    for (const Entry& entry : entries) {
        out << entry.website << " " << entry.user << " " << entry.encodedPass << "\n";
    }

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