#include "textCodeCipher.hpp"
#include "utils.hpp"
#include<fstream>
#include<iostream>

void TextCodeCipher::calculatePositions(){
    size_t len = (this->alphabetText).size();
    for(size_t i=0;i<largestASCII - smallestASCII+1; i++){
        positions[i] = -1;
    }
    for(size_t i=0;i<len;i++){
        if(positions[(int)alphabetText[i] - smallestASCII]==-1){
            positions[(int)alphabetText[i] - smallestASCII] = i;
        }
    }
}

EncryptedMessage TextCodeCipher::encrypt(const std::string& text){
    if(!pos_calculated){
        calculatePositions();
        pos_calculated = true;
    }

    int len = text.size();
    unsigned char* encrypted = new unsigned char[4*len];
    for(int i=0;i<len;i++){
        if(positions[(int)text[i] - smallestASCII]>=0){

            Utils::writeIntToBuffer(encrypted, positions[(int)text[i] - smallestASCII], i*sizeof(int32_t));
        }else{
            delete[] encrypted;
            throw std::invalid_argument("Character out of scope");
        }
    }

    /// encrypted is deleted in the destructor of EncryptedMessage
    /// The constructor of EncryptedMessage directly steals the pointer
    return EncryptedMessage(encrypted, 4*len, len);
}

std::string TextCodeCipher::decrypt(const EncryptedMessage& msg){

    char* dec = new char[msg.getLenOriginal()];

    for(size_t i=0;i<msg.getLenOriginal();i++){
        int val = Utils::readIntFromBuffer(msg.getEncrypted(), i*sizeof(int32_t));
        dec[i] = alphabetText[val];
    }

    std::string decrypted(dec, msg.getLenOriginal());
    delete[] dec;

    return decrypted;
}

void TextCodeCipher::loadAlphabetFromFile() {
  std::ifstream in(textFilePath);
  if (!in) throw std::runtime_error("Cannot open text‐alphabet file: " + textFilePath);
  std::string s;
  std::getline(in, s, '\0');  
  alphabetText = std::move(s);
}

bool TextCodeCipher::validateConfig(const std::string& config){
    
    std::ifstream file(config, std::ios::binary); /// binary mode is more stable for size checks

    if (!file.is_open()) {
        throw std::invalid_argument("File does not exist or can't be opened!");
    }

    // Check if file has any content
    file.seekg(0, std::ios::end);
    return file.tellg() > 0;
}

Cipher* TextCodeCipher::createCipherFromConfig(const std::string& config) {
    /// Any function that uses this has the obligation to take care to delete it
    /// This in reality is used only in the PasswordFile class
    return new TextCodeCipher(config, Source::FromFile);
}