#include "passwordManager.hpp"
#include "passwordFile.hpp"
#include "caesarCipher.hpp"
#include "textCodeCipher.hpp"
#include "hillCipher.hpp"
#include "utils.hpp"

#include<fstream>
#include<iostream>

void PasswordManager::create(const std::string& fileName, const std::string& cipherType,
                             const std::string& password, const std::string& cipherArgs) {

    std::string fileNameNew = Utils::fixTxtExtension(fileName);
    /// This is done in order to impose that the file open will be .txt
    /// Also this way the code will work the same nomatter if the user specifies .txt extension
    /// It would be interesting what would happen if a user tries to create simultaneously file, file.txt, file.txt.txt etc.
    /// However, I believe the system is robust to this, i.e. file and file.txt will be treated the same and all other variations will be treated differently. Testing shows no issues arise

    if (nameTaken(fileNameNew)) {
        /// We cannot allow multiple files with the same name in PasswordManager
        /// This is coherent with the fixing of the extension .txt
        throw std::runtime_error("File with this name already exists.");
    }

    /// We validate the cipher arguments before actually creating the file
    bool validity;
    try {
        validity = Cipher::validateConfig(cipherType, cipherArgs);
    } catch (const std::exception& e) {
        throw std::invalid_argument("Config not valid for cipher type " + cipherType + ":" + e.what());
    }
    if (!validity) throw std::invalid_argument("Config not valid for cipher type " + cipherType);

    /// Once the configuration is validated, we create the file
    /// We ensured fileNameNew has a .txt extension
    std::ofstream fileOut(fileNameNew);
    if (!fileOut) {
        throw std::runtime_error("Failed to create file: " + fileNameNew);
    }
    fileOut << "CIPHER " << cipherType << "\n";
    fileOut << "PASSWORD " << password << "\n";
    fileOut << "ARGS " << cipherArgs << "\n";
    fileOut.close();

    /// We create a PasswordFile* entry on the heap and add it to the PasswordManager
    PasswordFile* newFile = new PasswordFile(fileNameNew, cipherType, password, cipherArgs);

    newFile->setCipher(Cipher::createCipherFromConfig(cipherType, cipherArgs));

    files.push_back(newFile);

    std::cout << "File '" << fileNameNew << "' created successfully.\n";
}

void PasswordManager::open(const std::string& fileName, const std::string& password){

    size_t numFiles = files.size();
    
    /// We take care of the .txt extension of the fileName we want to open
    std::string fileNameNew = Utils::fixTxtExtension(fileName);

    /// We iterate over the PasswordFile objects and look for a match with the file we want to open
    for(size_t i=0;i<numFiles;i++){
        if(files[i]->getName() == fileNameNew){
            if(files[i]->getPassword() == password){
                /// We store the index of the current open file as an attribute to our PasswordManager
                current_open_idx = i;
                std::cout<<"File '" + files[i]->getName() +"' opened successfully\n";
                return;
            }else{
                /// If password from user is incorrect we do not grant access to the file
                throw std::invalid_argument("Invalid password -- you have no permission to open this file!");
                return;
            }
        }
    }

    /// If no file with the desired name exists, we let the user know
    throw std::runtime_error("File not found! Command unsuccessful!");
}

void PasswordManager::save(const std::string& website, const std::string& user, const std::string& pass) {
    
    /// We check whether any file is currently open
    if (current_open_idx<0) throw std::runtime_error("No password file is open.");
    
    if(pass.empty()) throw std::invalid_argument("Password for user and website can not be empty.");

    std::string encodedPass;
    try {
        EncryptedMessage encoded = files[current_open_idx]->getCipherPtr()->encrypt(pass);
        encodedPass = encoded.serialize(Cipher::encryptedAsString(files[current_open_idx]->getID()));
    } catch(const std::exception& exc) {
        throw std::runtime_error(std::string("Failed to encrypt password!") + exc.what());
    }

    /// Below we ensure that if we try to save a password for a user and website that already exist, we ask the user what to do. If they decide, we update the password, otherwise keep it.
    std::vector<Entry>& entries = files[current_open_idx]->getEntries();
    bool found = false;
    for (Entry& entry : entries) {
        if (entry.website == website && entry.user == user) {
            found = true;
            std::cout << "Password for this user and site already exists. Would you like to update it? [Y/N] \n";
            std::string answer;
            std::getline(std::cin, answer);
            if (answer == "Y" || answer == "y") {
                entry.encodedPass = encodedPass;
                std::cout << "Password updated.\n";
            } else {
                std::cout << "Password not changed.\n";
            }
            break;
        }
    }
    if (!found) {
        entries.push_back({website, user, encodedPass});
    }

    std::ofstream out(files[current_open_idx]->getName());
    if (!out) throw std::runtime_error("Failed to open password file for writing.");

    out << "CIPHER " << files[current_open_idx]->getID() << "\n";
    out << "PASSWORD " << files[current_open_idx]->getPassword() << "\n";
    out << "ARGS " << files[current_open_idx]->getConfig() << "\n";
    for (const Entry& entry : entries) {
        out << entry.website << " " << entry.user << " " << entry.encodedPass << "\n";
    }

}

void PasswordManager::load(const std::string& website, const std::string& user) {
    
    /// We check whether a file is currently open
    if (current_open_idx<0) {
        throw std::runtime_error("No password file is open.");
    }

    bool encryptedAsString = Cipher::encryptedAsString(files[current_open_idx]->getID());
    Cipher* cipher = files[current_open_idx]->getCipherPtr();
    const auto& entries = files[current_open_idx]->getEntries();
    /// We use auto because we need a read-only reference and this way we don't copy

    bool found = false;

    if (!user.empty()) {
        //  For Specific user:
        for (const Entry& entry : entries) {
            if (entry.website == website && entry.user == user) {
                EncryptedMessage enc = EncryptedMessage::deserialize(entry.encodedPass, encryptedAsString);
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
                EncryptedMessage enc = EncryptedMessage::deserialize(entry.encodedPass, encryptedAsString);
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

    if(newPass.empty()) throw std::invalid_argument("Password for user and website can not be empty.");


    std::vector<Entry>& entries = files[current_open_idx]->getEntries();
    Cipher* cipher = files[current_open_idx]->getCipherPtr();
    bool encryptedAsString = Cipher::encryptedAsString(files[current_open_idx]->getID());
    bool found = false;

    for (Entry& entry : entries) {
        if (entry.website == website && entry.user == user) {
            found = true;

            // Encrypt new password as would be stored
            EncryptedMessage newEnc = cipher->encrypt(newPass);
            std::string newEncoded = newEnc.serialize(encryptedAsString);

            // If new enc matches the old one, don't allow update:
            if (entry.encodedPass == newEncoded) {
                std::cout << "New password cannot be the same as the old password for site " << website << ", user " << user << ".\n";
                return;
            }

            // Otherwise, update and write all back to file
            entry.encodedPass = newEncoded;

            // Save everything back to the file
            std::ofstream out(files[current_open_idx]->getName());
            if (!out) throw std::runtime_error("Failed to open password file for writing.");

            out << "CIPHER " << files[current_open_idx]->getID() << "\n";
            out << "PASSWORD " << files[current_open_idx]->getPassword() << "\n";
            out << "ARGS " << files[current_open_idx]->getConfig() << "\n";
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

    std::vector<Entry>& entries = files[current_open_idx]->getEntries();

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

        /// We iterate backwards for safer deletion
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
    std::ofstream out(files[current_open_idx]->getName());
    if (!out) throw std::runtime_error("Failed to open password file for writing.");

    out << "CIPHER " << files[current_open_idx]->getID() << "\n";
    out << "PASSWORD " << files[current_open_idx]->getPassword() << "\n";
    out << "ARGS " << files[current_open_idx]->getConfig() << "\n";
    for (const Entry& e : entries) {
        out << e.website << " " << e.user << " " << e.encodedPass << "\n";
    }
}

bool PasswordManager::nameTaken(const std::string& name){
    size_t numFiles = files.size();
    for(size_t i=0;i<numFiles;i++){
        if(files[i]->getName() == name){
            return true;
        }
    }
    return false;
}