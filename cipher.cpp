#include "cipher.hpp"
#include "caesarCipher.hpp"
#include "textCodeCipher.hpp"
#include "hillCipher.hpp"

Cipher::~Cipher() {}

bool Cipher::validateConfig(const std::string& type, const std::string& config){
    if(type == "CAESAR") return CaesarCipher::validateConfig(config);
    else if(type == "TEXTCODE") return TextCodeCipher::validateConfig(config);
    else if(type == "HILL") return HillCipher::validateConfig(config);
    else throw std::invalid_argument("Unknown cipher type: " + type);
}

Cipher* Cipher::createCipherFromConfig(const std::string& type, const std::string& config){
    if(type == "CAESAR") return CaesarCipher::createCipherFromConfig(config);
    else if(type == "TEXTCODE") return TextCodeCipher::createCipherFromConfig(config);
    else if(type == "HILL") return HillCipher::createCipherFromConfig(config);
    else throw std::invalid_argument("Unknown cipher type: " + type);
}

bool Cipher::encryptedAsString(const std::string& type){
    if(type == "CAESAR") return true;
    else if(type == "TEXTCODE") return false;
    else if(type == "HILL") return false;
    else throw std::invalid_argument("Unknown cipher type: " + type);
}

