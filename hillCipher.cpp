#include "hillCipher.hpp"
#include "utils.hpp"
#include "matrixSquare.hpp"
#include<string>
#include<vector>
#include<fstream>
#include<iostream>

EncryptedMessage HillCipher::encrypt(const std::string& text){
    size_t n = this->matrix.get_size();
    size_t padded_len = Utils::get_padded(text.size(), n);

    std::string text_padded = text + std::string(padded_len - text.size(),'X');

    std::vector<int> vector_padded = Utils::asciiCodesFromString(text_padded);

    Matrix col_text = Matrix(vector_padded, Orientation::Column);

    MatrixSquare key = matrix.blockDiag(padded_len/n);

    Matrix res = key*col_text;

    unsigned char* encrypted = new unsigned char[4*padded_len];
    
    for(size_t i=0;i<padded_len;i++){
        Utils::writeIntToBuffer(encrypted, res[i][0], i*sizeof(int32_t));
    }

    return EncryptedMessage(encrypted,4*padded_len, text.size());

}
std::string HillCipher::decrypt(const EncryptedMessage& msg){
    std::vector<int> encrypted_vec;
    for(size_t i=0;i<msg.getLen()/4;i++){
        int val = Utils::readIntFromBuffer(msg.getEncrypted(), i*sizeof(int32_t));
        encrypted_vec.push_back(val);
    }

    Matrix enc(encrypted_vec, Orientation::Column);

    MatrixSquare inverse = matrix.inverseMod95();

    inverse = inverse.blockDiag(msg.getLen()/(4*matrix.get_rows()));

    Matrix res = inverse*enc;

    std::string result; 
    result.reserve(msg.getLen()/4);

    for(size_t i=0;i<msg.getLenOriginal();i++){
        result+=static_cast<char>(Utils::mod95ToAscii(res[i][0]));
    }

    return result;
}

bool HillCipher::validateConfig(const std::string& config){
    
    std::ifstream file(config);
    if (!file.is_open()) {
        throw std::invalid_argument("File does not exist!") ;
    }

    std::string line;

    // Read the first line (matrix size n)
    if (!std::getline(file, line)) {
        throw std::invalid_argument("First line of file is empty!") ;
    }
    int n = 0;
    try {
        size_t pos;
        n = std::stoi(line, &pos);
        if (pos != line.length()) {
          throw std::invalid_argument("Too many arguments on line 1!") ;
        }
    } catch (...) {
        throw std::invalid_argument("Could not get matrix size!") ;
    }
    if (n <= 0){
      throw std::invalid_argument("Matrix size is negative!") ;
    }

    // Parse the next n lines as matrix
    std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        if (!std::getline(file, line)) return false;

        size_t start = 0;
        for (int j = 0; j < n; ++j) {
            // Skip leading spaces
            while (start < line.length() && line[start] == ' ') start++;
            if (start >= line.length()) return false;
            // Find the end of the number
            size_t end = start;
            if (line[end] == '-' || line[end] == '+') ++end;
            while (end < line.length() && std::isdigit(line[end])) ++end;
            if (start == end) return false;
            try {
                int v = std::stoi(line.substr(start, end - start));
                matrix[i][j] = ((v % 95) + 95) % 95;
            } catch (...) {
                return false;
            }
            start = end;
        }
        // Check for any extra junk on the line
        while (start < line.length() && line[start] == ' ') ++start;
        if (start < line.length()) return false;
    }

    // Try to compute inverse (if not invertible: fail)
    
    MatrixSquare sq(matrix);
    if(sq.determinantMod95() == 0) throw std::invalid_argument("Matrix not invertible!");

    return true;
}

Cipher* HillCipher::createCipherFromConfig(const std::string& config) {
    /// Config should already be validated

    std::ifstream file(config);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open Hill cipher config file: " + config);
    }

    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Hill cipher config: empty file");
    }

    int n = 0;
    try {
        size_t pos;
        n = std::stoi(line, &pos);
        if (pos != line.length()) throw std::runtime_error("Extra characters after matrix size");
    } catch (...) {
        throw std::runtime_error("Could not parse matrix size in HillCipher config");
    }
    if (n <= 0) throw std::runtime_error("Matrix size must be positive");

    std::vector<std::vector<int>> mat(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Not enough rows in Hill cipher config for matrix of size " + std::to_string(n));
        }
        size_t start = 0;
        for (int j = 0; j < n; ++j) {
            // Skip leading spaces
            while (start < line.size() && std::isspace(line[start])) ++start;
            if (start >= line.size()) {
                throw std::runtime_error("Not enough columns on line " + std::to_string(i+1));
            }

            // Find end of the current number
            size_t end = start;
            // Allow + or -
            if (end < line.size() && (line[end] == '-' || line[end] == '+')) ++end;
            while (end < line.size() && std::isdigit(line[end])) ++end;
            if (start == end) {
                throw std::runtime_error("Missing integer at row " + std::to_string(i+1) + ", col " + std::to_string(j+1));
            }
            try {
                mat[i][j] = std::stoi(line.substr(start, end - start));
                mat[i][j] = ((mat[i][j] % 95) + 95) % 95;
            } catch (...) {
                throw std::runtime_error("Invalid integer at row " + std::to_string(i+1) + ", col " + std::to_string(j+1));
            }
            start = end;
        }
        // Check for extra non-space stuff
        while (start < line.size() && std::isspace(line[start])) ++start;
        if (start < line.size()) {
            throw std::runtime_error("Too many columns on line " + std::to_string(i+1));
        }
    }

    MatrixSquare key(mat);

    // Check invertibility
    try {
        key.inverseMod95();
    } catch (...) {
        throw std::invalid_argument("Matrix is not invertible modulo 95 in Hill cipher config.");
    }

    return new HillCipher(key);
}