#include "petkovaCipher.hpp"
#include "utils.hpp"
#include<iostream>


EncryptedMessage PetkovaCipher::encrypt(const std::string& text) {
    std::string numStr = std::to_string(num);
    size_t resLen = text.size() * (1 + numStr.size());
    size_t sz = numStr.size();
    size_t textSz = text.size();
    unsigned char* result = new unsigned char[resLen];
    for(size_t i=0; i<textSz; i++){
        result[i*(1+sz)] = text[i];
        for(size_t j=0;j<sz;j++){
            result[i*(1+sz)+j+1] = numStr[j];
        }
    }
    // std::cout<<"Encryption stuff: "<<text.size() * (1 + numStr.size())<<"\n";
    // std::cout<<"Encryption stuff original: "<<textSz<<"\n";

    EncryptedMessage temp = EncryptedMessage(result, text.size() * (1 + numStr.size()), textSz);

    // std::cout<<"Encryption stuff original: "<<temp.getLenOriginal()<<"\n";
    
    return temp;
}

std::string PetkovaCipher::decrypt(const EncryptedMessage& msg){
    /// Decryptes the message by shifting the character by the negative shift
    /// This allows us to reuse the code for chifting a single character
    std::string temp(reinterpret_cast<const char*>(msg.getEncrypted()), msg.getLen());

    std::string numStr = std::to_string(num);
    size_t sz = numStr.size();

    size_t len_o = msg.getLen()/(sz+1);
    std::string decrypted(len_o, ' ');

    for(size_t i=0;i<len_o;i++){
        decrypted[i] = temp[(sz+1)*i];
        std::cout<<decrypted[i];
    }
    std::cout<<"Decrypted successfully\n";

    return decrypted;
}

bool PetkovaCipher::validateConfig(const std::string& config){
    if (config.empty()) return false;

    size_t start = 0;
    if (config[0] == '-') {
        /// Supporting negative numbers
        if (config.size() == 1) return false; // just "-"
        start = 1;
    }

    for (size_t i = start; i < config.size(); ++i) {
        if (!std::isdigit(config[i])) return false;
    }

    try {
        int val = std::stoi(config);
        return true;
    } catch (...) {
        /// Could not convert value to int -- probably too big
        return false;
    }
}

Cipher* PetkovaCipher::createCipherFromConfig(const std::string& config) {
    int num = std::stoi(config);
    return new PetkovaCipher(num);
}