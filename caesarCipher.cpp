#include "caesarCipher.hpp"
#include<iostream>

int CaesarCipher::eval_casted_shift(int shift){
    /// Casts shift to a remainder modulo RANGE. This is to be able to handle both negative shifts and ones that overflow RANGE
    int r = shift % RANGE;  
    if (r < 0) r += RANGE; 
    return r;
}

void CaesarCipher::setShift(int newShift){
    shift = eval_casted_shift(newShift);
}

char CaesarCipher::shiftChar(char ch, int shift){
    /// Shifts a character by shift positions
    int x = ch - smallestASCII;
    x = (x + shift) % RANGE;
    return char(x + smallestASCII);
}

EncryptedMessage CaesarCipher::encrypt(const std::string& text){
    /// Encrypts a message by shifting the characters one by one
    size_t len = text.size();
    unsigned char* encrypted = new unsigned char [len];

    for(size_t i=0;i<len;i++){
        encrypted[i] = shiftChar(text[i], shift);
    }
    return EncryptedMessage(encrypted, len, len);
}

std::string CaesarCipher::decrypt(const EncryptedMessage& msg){
    /// Decryptes the message by shifting the character by the negative shift
    /// This allows us to reuse the code for chifting a single character
    std::string decrypted(reinterpret_cast<const char*>(msg.getEncrypted()), msg.getLen());

    for(size_t i=0;i<msg.getLen();i++){
        decrypted[i] = shiftChar(msg.getEncrypted()[i], eval_casted_shift(-shift));
    }
    return decrypted;
}

bool CaesarCipher::validateConfig(const std::string& config){
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

Cipher* CaesarCipher::createCipherFromConfig(const std::string& config) {
    int shift = std::stoi(config);
    return new CaesarCipher(shift);
}