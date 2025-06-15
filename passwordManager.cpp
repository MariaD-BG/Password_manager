#include "passwordManager.hpp"
#include "passwordFile.hpp"

#include<fstream>
#include<iostream>

void PasswordManager::create(const std::string& fileName, const std::string& cipherType, const std::string& password, const std::string& cipherArgs) {
    if (nameTaken(fileName)) {
        throw std::runtime_error("File with this name already exists.");
    }

    // Create and open the actual file
    std::ofstream fileOut(fileName);  // Can add extension if needed: fileName + ".pw"
    if (!fileOut) {
        throw std::runtime_error("Failed to create file: " + fileName);
    }

    // Write metadata to file (this depends on your storage format!)
    fileOut << "CIPHER " << cipherType << "\n";
    fileOut << "PASSWORD " << password << "\n";  // Ideally hashed/encrypted
    fileOut << "ARGS " << cipherArgs << "\n";
    fileOut.close();

    // Now create a PasswordFile object and store it
    PasswordFile newFile(fileName, cipherType, password, cipherArgs);
    files.push_back(std::move(newFile));

    std::cout << "File '" << fileName << "' created successfully.\n";
}

void PasswordManager::open(const std::string& fileName, const std::string& password){
    size_t numFiles = files.size();
    std::cerr<<"There are "<<numFiles<<" files in collection\n";
    for(size_t i=0;i<numFiles;i++){
        if(files[i].getName() == fileName){
            if(files[i].getPassword() == password){
                current_open = &files[i];
                std::cout<<"File opened successfully!\n";
                return;
            }else{
                std::cerr<<"Invalid password -- you have no permission to open thsi file!\n";
                /// TODO: remove above -- testing purposes only
                throw "Invalid password -- you have no permission to open thsi file!";
                return;
            }
        }
    }
    std::cerr<<"File not found! Command unsuccessful!\n";
    throw "File not found! Command unsuccessful!";
    /// TODO: remove above -- testing purposes only
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