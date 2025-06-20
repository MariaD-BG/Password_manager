# pragma once
#include <stddef.h>
#include<string>
#include<cstring>
#include<vector>
#include <stdexcept>

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

    std::string serialize(bool asString) const {
        if (asString) {
            return std::string(reinterpret_cast<const char*>(encrypted), len);
        } else {
            std::string result = std::to_string(getLenOriginal()) + "|";
            for (size_t i = 0; i < getLen(); ++i) {
                if (i > 0) result += ",";
                result += std::to_string(static_cast<unsigned int>(getEncrypted()[i]));
            }
            return result;
        }
    }

    static EncryptedMessage deserialize(const std::string& data, bool asString) {
        if (asString) {
            size_t len = data.size();
            unsigned char* buf = new unsigned char[len];
            for (size_t i = 0; i < len; ++i) buf[i] = static_cast<unsigned char>(data[i]);
            return EncryptedMessage(buf, len, len);
        } else {
            // Parsing "<originalLen>|num1,num2,num3,..."
            size_t bar = data.find('|');
            if (bar == std::string::npos)
                throw std::runtime_error("Invalid EncryptedMessage: missing |");
            size_t lenOriginal = std::stoul(data.substr(0, bar));
            std::vector<unsigned char> bytes;
            size_t start = bar + 1, end;

            while (start < data.size()) {
                end = data.find(',', start);
                std::string numstr = (end == std::string::npos) ? data.substr(start) : data.substr(start, end - start);
                int n = std::stoi(numstr);
                if (n < 0 || n > 255) throw std::runtime_error("Invalid byte in EncryptedMessage");
                bytes.push_back(static_cast<unsigned char>(n));
                if (end == std::string::npos) break;
                start = end + 1;
            }
            unsigned char* arr = new unsigned char[bytes.size()];
            for (size_t i = 0; i < bytes.size(); ++i) arr[i] = bytes[i];
            return EncryptedMessage(arr, bytes.size(), lenOriginal);
        }
    }


private:
    unsigned char* encrypted;
    size_t len;
    size_t len_original;
};