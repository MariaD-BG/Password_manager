#pragma once
#include<string>
#include "encryptedMessage.hpp"

class Cipher {
public:

    virtual EncryptedMessage encrypt(const std::string& text) = 0;
    virtual std::string decrypt(const EncryptedMessage& msg) = 0;
    virtual ~Cipher() = 0;

    virtual std::string id() const = 0;

    static bool validateConfig(const std::string& type, const std::string& config);
    static Cipher* createCipherFromConfig(const std::string& type, const std::string& config);
    static bool encryptedAsString(const std::string& type);

protected:
    static constexpr unsigned int smallestASCII = 32;
    static constexpr unsigned int largestASCII = 126;
    static constexpr int RANGE = largestASCII - smallestASCII +1;
};
