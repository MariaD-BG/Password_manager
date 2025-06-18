#include "hillCipher.hpp"
#include "utils.hpp"
#include "matrixSquare.hpp"
#include<string>
#include<vector>
#include<iostream>

EncryptedMessage HillCipher::encrypt(const std::string& text){
    size_t n = this->matrix.get_size();
    size_t padded_len = Utils::get_padded(text.size(), n);

    std::string text_padded = text + std::string(padded_len - text.size(),'X');

    std::vector<int> vector_padded = Utils::asciiCodesFromString(text_padded);

    Matrix col_text = Matrix(vector_padded, Orientation::Column);
    
    matrix.print();

    MatrixSquare key = matrix.blockDiag(padded_len/n);

    Matrix res = key*col_text;

    unsigned char* encrypted = new unsigned char[4*padded_len];
    
    for(size_t i=0;i<padded_len;i++){
        Utils::writeIntToBuffer(encrypted, res[i][0], i*sizeof(int32_t));
    }

    std::cout<<"Returning from encrypt\n";
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
        std::cout<<res[i][0]<<" ";
        result+=static_cast<char>(Utils::mod95ToAscii(res[i][0]));
    }

    return result;
}

void HillCipher::saveConfig(std::ostream& out) const {
  int n = int(matrix.get_size());
  out << n << "\n";
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      out << matrix[i][j]
          << (j + 1 < n ? ' ' : '\n');
    }
  }
}

void HillCipher::loadConfig(std::istream& in) {
  int n;
  if (!(in >> n)) 
    throw std::runtime_error("HillCipher::loadConfig: couldn't read dimension");

  {
    std::string dummy;  
    std::getline(in, dummy);
  }
  
  std::vector<std::vector<int>> mat(n, std::vector<int>(n));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (!(in >> mat[i][j]))
        throw std::runtime_error("HillCipher::loadConfig: couldn't read matrix entry");
      
      mat[i][j] %= 95;
      if (mat[i][j] < 0) mat[i][j] += 95;
    }
    
    std::string dummy;
    std::getline(in, dummy);
  }

  matrix = MatrixSquare(mat);

}

bool HillCipher::validateConfig(const std::string& config){

  
}