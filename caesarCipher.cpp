#include "caesarCipher.hpp"
#include<iostream>

int CaesarCipher::eval_casted_shift(int shift){
    int r = shift % RANGE;  
    if (r < 0) r += RANGE; 
    return r;
}

void CaesarCipher::setShift(int newShift){
    shift = eval_casted_shift(newShift);
}

char CaesarCipher::shiftChar(char ch, int shift){
    int x = ch - smallestASCII;
    x = (x + shift) % RANGE;
    return char(x + smallestASCII);
}

EncryptedMessage CaesarCipher::encrypt(const std::string& text){
    size_t len = text.size();

    unsigned char* encrypted = new unsigned char [len];

    for(size_t i=0;i<len;i++){
        encrypted[i] = shiftChar(text[i], shift);
    }

    return EncryptedMessage(encrypted, len, len);
}

std::string CaesarCipher::decrypt(const EncryptedMessage& msg){

    std::string decrypted(reinterpret_cast<const char*>(msg.getEncrypted()), msg.getLen());

    for(size_t i=0;i<msg.getLen();i++){
        decrypted[i] = shiftChar(msg.getEncrypted()[i], eval_casted_shift(-shift));
    }
    return decrypted;
}

void CaesarCipher::saveConfig(std::ostream& out) const{
    out<<shift<<"\n";
}
void CaesarCipher::loadConfig(std::istream& in) {
    int s;
    if (!(in >> s))
        throw std::runtime_error("Failed to read Caesar shift");
    // discard everything up to and including the end of this line
    std::string dump;
    std::getline(in, dump);
    shift = eval_casted_shift(s);
}