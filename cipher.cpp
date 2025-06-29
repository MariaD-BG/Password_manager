#include "cipher.hpp"
#include "caesarCipher.hpp"
#include "textCodeCipher.hpp"
#include "hillCipher.hpp"
#include "petkovaCipher.hpp"

Cipher::~Cipher() {}

/// The idea here is to resembe a static factory pattern

bool Cipher::validateConfig(const std::string& type, const std::string& config){
    if(type == "CAESAR") return CaesarCipher::validateConfig(config);
    else if(type == "TEXTCODE") return TextCodeCipher::validateConfig(config);
    else if(type == "HILL") return HillCipher::validateConfig(config);
    else if(type == "PETKOVA") return PetkovaCipher::validateConfig(config);
    else throw std::invalid_argument("Unknown cipher type: " + type);
}

/// A central factory function (using if/else) selects which derived cipher to construct based on runtime data, and then returns a pointer to the base class

Cipher* Cipher::createCipherFromConfig(const std::string& type, const std::string& config){
    if(type == "CAESAR") return CaesarCipher::createCipherFromConfig(config);
    else if(type == "TEXTCODE") return TextCodeCipher::createCipherFromConfig(config);
    else if(type == "HILL") return HillCipher::createCipherFromConfig(config);
    else if(type == "PETKOVA") return PetkovaCipher::createCipherFromConfig(config);
    else throw std::invalid_argument("Unknown cipher type: " + type);
}

bool Cipher::encryptedAsString(const std::string& type){
    if(type == "CAESAR") return true;
    else if(type == "TEXTCODE") return false;
    else if(type == "HILL") return false;
    else if(type == "PETKOVA") return true;
    else throw std::invalid_argument("Unknown cipher type: " + type);
}

