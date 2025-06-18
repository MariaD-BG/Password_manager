#pragma once
#include "cipher.hpp"
#include "matrixSquare.hpp"

class HillCipher : public Cipher{
public:
    HillCipher(MatrixSquare matrix) : matrix(matrix) {}
    EncryptedMessage encrypt(const std::string& text) override;
    std::string decrypt(const EncryptedMessage& msg) override;
    ~HillCipher() override {}

    virtual std::string id() const { return "HILL"; }
    virtual void saveConfig(std::ostream& out) const;
    virtual void loadConfig(std::istream& in);

    static bool validateConfig(const std::string& config);
    static Cipher* createCipherFromConfig(const std::string& config);


private:
    MatrixSquare matrix;
    // static constexpr unsigned int mod_const = largestASCII -  smallestASCII +1 +1;
};
