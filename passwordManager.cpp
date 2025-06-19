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

    for(size_t i=0;i<numFiles;i++){
        if(files[i].getName() == fileNameNew){
            if(files[i].getPassword() == password){
                current_open_idx = i;
                std::cout<<"File " + files[i].getName() +" opened successfully\n";
                return;
            }else{
                throw std::invalid_argument("Invalid password -- you have no permission to open this file!\n");
                return;
            }
        }
    }

    throw std::runtime_error("File not found! Command unsuccessful!");
}

void PasswordManager::save(const std::string& website, const std::string& user, const std::string& pass) {
    
    if (current_open_idx<0) throw std::runtime_error("No password file is open.");
    
    std::string encodedPass;
    try {
        EncryptedMessage encoded = files[current_open_idx].getCipherPtr()->encrypt(pass);
        if(files[current_open_idx].getID() == "CAESAR") encodedPass = encoded.serialize(true); 
        else encodedPass = encoded.serialize(false);
    } catch(const std::exception& exc) {
        throw std::runtime_error(std::string("Failed to encrypt password!") + exc.what());
    }

    std::vector<Entry>& entries = files[current_open_idx].getEntries();
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

    std::ofstream out(files[current_open_idx].getName());
    if (!out) throw std::runtime_error("Failed to open password file for writing.");

    out << "CIPHER " << files[current_open_idx].getID() << "\n";
    out << "PASSWORD " << files[current_open_idx].getPassword() << "\n";
    out << "ARGS " << files[current_open_idx].getConfig() << "\n";
    for (const Entry& entry : entries) {
        out << entry.website << " " << entry.user << " " << entry.encodedPass << "\n";
    }

}

void PasswordManager::load(const std::string& website, const std::string& user) {
    if (current_open_idx<0) {
        throw std::runtime_error("No password file is open.");
    }

    bool isCaesar = (files[current_open_idx].getID() == "CAESAR");
    Cipher* cipher = files[current_open_idx].getCipherPtr();
    const auto& entries = files[current_open_idx].getEntries();

    bool found = false;

    if (!user.empty()) {
        //  For Specific user:
        for (const Entry& entry : entries) {
            if (entry.website == website && entry.user == user) {
                EncryptedMessage enc = EncryptedMessage::deserialize(entry.encodedPass, isCaesar);
                std::string decrypted = cipher->decrypt(enc);
                std::cout << "Password for " << website << ", user " << user << ": " << decrypted << "\n";
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "No password found for site " << website << " and user " << user << ".\n";
        }
    } else {
        // If no user is specified, show all for site
        for (const Entry& entry : entries) {
            if (entry.website == website) {
                EncryptedMessage enc = EncryptedMessage::deserialize(entry.encodedPass, isCaesar);
                std::string decrypted = cipher->decrypt(enc);
                std::cout << "User: " << entry.user << " Password: " << decrypted << "\n";
                found = true;
            }
        }
        if (!found) {
            std::cout << "No users found for site " << website << ".\n";
        }
    }
}

void PasswordManager::update(const std::string& website, const std::string& user, const std::string& newPass) {
    if (current_open_idx<0) {
        std::cout << "No password file is open.\n";
        return;
    }

    std::vector<Entry>& entries = files[current_open_idx].getEntries();
    Cipher* cipher = files[current_open_idx].getCipherPtr();
    bool isCaesar = (files[current_open_idx].getID() == "CAESAR");
    bool found = false;

    for (Entry& entry : entries) {
        if (entry.website == website && entry.user == user) {
            found = true;

            // Encrypt new password as would be stored
            EncryptedMessage newEnc = cipher->encrypt(newPass);
            std::string newEncoded = newEnc.serialize(isCaesar);

            // If new enc matches the old one, don't allow update:
            if (entry.encodedPass == newEncoded) {
                std::cout << "New password cannot be the same as the old password for site " << website << ", user " << user << ".\n";
                return;
            }

            // Otherwise, update and write all back to file
            entry.encodedPass = newEncoded;

            // Save everything back to the file
            std::ofstream out(files[current_open_idx].getName());
            if (!out) throw std::runtime_error("Failed to open password file for writing.");

            out << "CIPHER " << files[current_open_idx].getID() << "\n";
            out << "PASSWORD " << files[current_open_idx].getPassword() << "\n";
            out << "ARGS " << files[current_open_idx].getConfig() << "\n";
            for (const Entry& e : entries) {
                out << e.website << " " << e.user << " " << e.encodedPass << "\n";
            }

            std::cout << "Password updated for site " << website << ", user " << user << ".\n";
            return;
        }
    }

    // If not found:
    if (!found) {
        std::cout << "No password found for site " << website << " and user " << user << ".\n";
    }
}

void PasswordManager::del(const std::string& website, const std::string& user) {
    if (current_open_idx<0) {
        std::cout << "No password file is open.\n";
        return;
    }

    std::vector<Entry>& entries = files[current_open_idx].getEntries();

    if (!user.empty()) {
        // Delete a single (website, user) entry
        bool found = false;
        for (size_t i = 0; i < entries.size(); ++i) {
            if (entries[i].website == website && entries[i].user == user) {
                entries.erase(entries.begin() + i);
                std::cout << "Deleted password for " << website << ", user " << user << ".\n";
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "No password found for site " << website << " and user " << user << ".\n";
            return;
        }
    } else {
        // Delete all entries for a website
        bool anyDeleted = false;
        // Iterate backwards for safe erasing
        for (int i = static_cast<int>(entries.size()) - 1; i >= 0; --i) {
            if (entries[i].website == website) {
                entries.erase(entries.begin() + i);
                anyDeleted = true;
            }
        }
        if (!anyDeleted) {
            std::cout << "No passwords found for site " << website << ".\n";
            return;
        } else {
            std::cout << "Deleted all passwords for site " << website << ".\n";
        }
    }

    // Rewrite the password file 
    std::ofstream out(files[current_open_idx].getName());
    if (!out) throw std::runtime_error("Failed to open password file for writing.");

    out << "CIPHER " << files[current_open_idx].getID() << "\n";
    out << "PASSWORD " << files[current_open_idx].getPassword() << "\n";
    out << "ARGS " << files[current_open_idx].getConfig() << "\n";
    for (const Entry& e : entries) {
        out << e.website << " " << e.user << " " << e.encodedPass << "\n";
    }
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