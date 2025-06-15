# pragma once
#include <stddef.h>
#include<cstring>

class EncryptedMessage{
public:
    EncryptedMessage(const EncryptedMessage&)            = delete;
    EncryptedMessage& operator=(const EncryptedMessage&) = delete;

    EncryptedMessage& operator=(EncryptedMessage&&) = delete;
    
    EncryptedMessage() : encrypted(nullptr), len(0), len_original(0){}

    EncryptedMessage(unsigned char* encrypted1, size_t len1, size_t len_original1) : encrypted(encrypted1), len(len1), len_original(len_original1){}

    EncryptedMessage(EncryptedMessage&& other) noexcept
        : encrypted(other.encrypted), len(other.len), len_original(other.len_original) {
        other.encrypted = nullptr;
        other.len = 0;
        other.len_original = 0;
        /// Leaving this here so that EncryptedMessage e = cipher.encrypt(input) would work even under C++14 or earlier
    }

    ~EncryptedMessage(){
        delete[] encrypted;
    }

    unsigned char* getEncrypted() const{
        return encrypted;
    }

    size_t getLen() const{
        return len;
    }

    size_t getLenOriginal() const{
        return len_original;
    }

private:
    unsigned char* encrypted;
    size_t len;
    size_t len_original;
};