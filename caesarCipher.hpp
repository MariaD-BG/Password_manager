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
    void saveConfig(std::ostream& out) const override;
    void loadConfig(std::istream& in) override;

private:
    int shift;
    char shiftChar(char ch, int shift);
    int char_type(char ch);
};
