#pragma once
#include "cipher.hpp"
#include "encryptedMessage.hpp"

class TextCodeCipher : public Cipher{
public:
    // TextCodeCipher(const std::string alphabetText) : alphabetText(alphabetText) {}

    enum class Source { FromFile, FromAlphabet };

    TextCodeCipher(const std::string& data, Source source){

        if(source==Source::FromFile){
            textFilePath = data;
            loadAlphabetFromFile();
        } else {
            alphabetText = data;
        }

        calculatePositions();
    }

    void loadAlphabetFromFile();

    EncryptedMessage encrypt(const std::string& text) override;
    std::string decrypt(const EncryptedMessage& msg) override;
    ~TextCodeCipher() {}

    void calculatePositions();

    std::string  id()       const override { return "TEXTCODE"; }
    void saveConfig(std::ostream& out) const override;
    void loadConfig(std::istream& in) override;

private:
    std::string alphabetText;
    std::string textFilePath;
    int positions[RANGE];
    bool pos_calculated = false;
};
