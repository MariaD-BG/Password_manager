#pragma once
#include "cipher.hpp"
#include "encryptedMessage.hpp"

class CaesarCipher : public Cipher {
public:
    CaesarCipher(int shift) : shift(eval_casted_shift(shift)) {}
    void setShift(int newShift);
    int eval_casted_shift(int shift);
    EncryptedMessage encrypt(const std::string& text) override;
    std::string decrypt(const EncryptedMessage& msg) override;
    ~CaesarCipher() override {}

    std::string  id()       const override { return "CAESAR"; }
        
    static bool validateConfig(const std::string& config);
    static Cipher* createCipherFromConfig(const std::string& config);

private:
    int shift;
    char shiftChar(char ch, int shift);
};
