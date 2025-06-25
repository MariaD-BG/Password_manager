#pragma once
#include "cipher.hpp"
#include "encryptedMessage.hpp"

class PetkovaCipher : public Cipher {
public:
    PetkovaCipher(int num) : num(num) {}
    EncryptedMessage encrypt(const std::string& text) override;
    std::string decrypt(const EncryptedMessage& msg) override;
    ~PetkovaCipher() override {}

    std::string id() const override { return "PETKOVA"; }
        
    static bool validateConfig(const std::string& config);
    static Cipher* createCipherFromConfig(const std::string& config);

private:
    int num;
};
