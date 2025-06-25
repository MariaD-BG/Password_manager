#pragma once
#include<string>
#include<cstring>
#include <cstdint>
#include<cassert>
#include<vector>
#include<iostream>

class Utils{
public:
    static void writeIntToBuffer(unsigned char* buffer, int32_t value, size_t offset) {
        std::memcpy(buffer + offset, &value, sizeof(int32_t));
    }

    static int32_t readIntFromBuffer(const unsigned char* buffer, size_t offset) {
        int32_t value;
        std::memcpy(&value, buffer + offset, sizeof(int32_t));
        return value;
    }

    template<typename T>
    static T min(const T& a, const T& b) {
        return (a < b) ? a : b;
        /// If this is called on an object of a class without a defined < operator, this will result in a
        /// compile-time error a.k.a not my problem
    }

    static int mod95(int x, const int mod = 95) {
        return (x % mod + mod) % mod;
    }

    static int modInverse95(int a, const int mod = 95) {
        a = mod95(a);
        for (int x = 1; x < mod; ++x)
            if ((a * x) % mod == 1)
                return x;
        return 0; /// No modular inverse exists because gcd(a,95)!=1
    }

    static int get_padded(int len, int mod){
        /// pads the length to a multiple of mod
        /// Used for HILL cipher
        return ((len + mod - 1) / mod) * mod;
    }

    static std::vector<int> asciiCodesFromString(const std::string& input) {
        
        /// Takes a string and returns a vector from the ascii codes
        
        std::vector<int> codes;
        codes.reserve(input.size()); 

        for (size_t i=0;i<input.size();i++) {
            codes.push_back(static_cast<unsigned char>(input[i]));
        }

        return codes;
    }

    static char mod95ToAscii(int residue, int smallestASCII = 32, int mod=95) {

        /// Takes an element from Z_95 and converts to ascii code
        /// Used for Hill decryption

        assert (residue >= 0 && residue < mod);

        if(residue>=smallestASCII) return static_cast<char>(residue);
        else return static_cast<char>(residue+mod);
    }

    static bool checkCipherID(std::string ID){
        if(ID!="HILL" && ID!="TEXTCODE" && ID!="CAESAR") return false;
        return true;
    }

    static std::string fixTxtExtension(const std::string& fileName) {
        if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".txt") {
            return fileName;
        } else {
            return fileName + ".txt";
        }
    }

    static std::string trim(const std::string& s) {
        std::string::const_iterator start = s.begin();
        while (start != s.end() && std::isspace(*start)) {
            ++start;
        }

        if (start == s.end()) {
            return ""; // string is all whitespace
        }

        std::string::const_iterator end = s.end();
        /// We use this to iterate over a string without modifying it
        do {
            --end;
        } while (end != start && std::isspace(*end));

        return std::string(start, end + 1);
    }

    static int numDig(int num){
        if(num == 0) return 1;
        int dig = 0;
        while(num>0){
            num = num/10;
            dig++;
        }
        return dig;
    }

    Utils() = delete; ///Static-only class: no instances
};
