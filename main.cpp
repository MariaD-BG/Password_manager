#include <iostream>
#include<fstream>
#include "caesarCipher.hpp"
#include "textCodeCipher.hpp"
#include "utils.hpp"
#include "matrix.hpp"
#include "matrixSquare.hpp"
#include "hillCipher.hpp"
#include "encryptedMessage.hpp"
#include "passwordManager.hpp"
#include "passwordFile.hpp"
#include "commandProcessor.hpp"

int main()
{


    // 1) Test CaesarCipher with shift=3
  {
    CaesarCipher c(3);
    std::string plain = "Hello, World!";
    EncryptedMessage em = c.encrypt(plain);
    std::string round = c.decrypt(em);

    std::cout << "=== Caesar Cipher (shift=3) ===\n";
    std::cout << "Plaintext:  " << plain << "\n";
    std::cout << "Encrypted:  ";
    // print each encrypted byte as an integer
    for (size_t i = 0; i < em.getLen(); ++i)
      std::cout << int(em.getEncrypted()[i]) << ' ';
    std::cout << "\nDecrypted:  " << round << "\n\n";
  }

  // 2) Prepare a file for TextCodeCipher
    const char* alphaFile = "alphabet.txt";
    {
        std::ofstream fout(alphaFile);
        if (!fout) {
            std::cerr << "Cannot create " << alphaFile << "\n";
            return 1;
        }
        fout << 
          "the brown fox jumps over the lazy dog "
          "THE BROWN FOX JUMPS OVER THE LAZY DOG";
    }

    // 3) Test TextCodeCipher from file
    {
        TextCodeCipher tc(alphaFile, TextCodeCipher::Source::FromFile);
        std::string plain = "TestVhod";
        auto em = tc.encrypt(plain);
        auto round = tc.decrypt(em);

        std::cout << "=== TextCodeCipher (from file) ===\n";
        std::cout << "Plaintext:  " << plain << "\n";
        // std::cout << "Codes:      ";
        // printCodes(em);
        std::cout << "\nDecrypted:  " << round << "\n\n";
    }

    // 4) Test TextCodeCipher from in‐memory alphabet
    {
        const std::string alpha =
          "the brown fox jumps over the lazy dog "
          "THE BROWN FOX JUMPS OVER THE LAZY DOG";

        TextCodeCipher tc(alpha, TextCodeCipher::Source::FromAlphabet);
        std::string plain = "TestVhod";
        auto em    = tc.encrypt(plain);
        auto round = tc.decrypt(em);

        std::cout << "=== TextCodeCipher (from string) ===\n";
        std::cout << "Plaintext:  " << plain << "\n";
        // std::cout << "Codes:      ";
        // printCodes(em);
        std::cout << "\nDecrypted:  " << round << "\n\n";
    }

    {
    std::cout << "=== Hill Cipher ===\n";

    std::vector<std::vector<int>> key = {
      { 3, 3 },
      { 2, 5 }
    };
    MatrixSquare ms(key);
    HillCipher hc(ms);

    std::string plain = "HELLO"; 
    std::cout << "Plaintext:  " << plain << "\n";

    EncryptedMessage em = hc.encrypt(plain);

    size_t count = em.getLen() / sizeof(int32_t);
    std::cout << "Encrypted codes: ";
    for (size_t i = 0; i < count; ++i) {
      int code = Utils::readIntFromBuffer(
                   em.getEncrypted(),
                   i * sizeof(int32_t)
                 );
      std::cout << code << ' ';
    }
    std::cout << "\n";

    // 5) decrypt back to the original length
    std::string round = hc.decrypt(em);
    std::cout << "Decrypted:  " << round << "\n";
    }

    PasswordManager& pm1 = PasswordManager::getInstance();
    // pm1.create("test.txt", "CAESAR", "password_test", "test_args");
    // pm1.open("test.txt", "password_test");

    std::cout<<"------------------";
    CommandProcessor processor(pm1); // Pass the reference
    processor.run();


    // CaesarCipher cipher1(3); // shift by 3
    // std::string input = "Hello---World";
    // EncryptedMessage encrypted1 = cipher1.encrypt(input);
    
    // std::string decrypted1 = cipher1.decrypt(encrypted1);
    // std::cout << "Original:  " << input << "\n";
    // std::cout << "Encrypted:  ";
    // for(size_t i=0;i<encrypted1.getLenOriginal();i++){
    //     std::cout<<encrypted1.getEncrypted()[i];
    // }
    // std::cout<<"\n";
    // std::cout << "Decrypted: " << decrypted1 << "\n";
    // std::cout<<"Final test: "<<cipher1.decrypt(cipher1.encrypt(input))<<"\n";

    // std::cout<<"---------------------\n";
    // TextCodeCipher cipher2("The quick brown fox jumps over the lazy dog!");
    // std::string input2 = "quicker quack my name is black";
    // std::cout<<"Original: "<<input2<<"\n";
    // EncryptedMessage encrypted2 = cipher2.encrypt(input2);
    // std::cout << "Encrypted:  ";
    // for(size_t i=0;i<encrypted2.getLenOriginal();i++){
    //     std::cout<<Utils::readIntFromBuffer(encrypted2.getEncrypted(), i*sizeof(int32_t))<<" ";
    // }
    // std::string decrypted2 = cipher2.decrypt(encrypted2);
    // std::cout<<"Decrypted: "<<decrypted2<<"\n";
    // std::cout<<"Final test: "<<cipher2.decrypt(cipher2.encrypt(input2))<<"\n";

    // std::cout<<"------------------------\n";

    // MatrixSquare m(3);
    // m[0][0] = 6;
    // m[0][1] = 24;
    // m[0][2] = 1;
    // m[1][0] = 13;
    // m[1][1] = 16;
    // m[1][2] = 10;
    // m[2][0] = 20;
    // m[2][1] = 17;
    // m[2][2] = 15;
 
    // HillCipher cipher3(m);

    // std::string input3 = "a_Bc";
    // std::cout<<"Original: "<<input3<<"\n";
    // EncryptedMessage encrypted3 = cipher3.encrypt(input3);
    // std::cout << "Encrypted:  \n";
    // for(size_t i=0;i<encrypted3.getLen()/4;i++){
    //     std::cout<<Utils::readIntFromBuffer(encrypted3.getEncrypted(), i*sizeof(int32_t))<<" ";
    // }
    // std::string decrypted3 = cipher3.decrypt(encrypted3);
    // std::cout<<"Decrypted: "<<decrypted3<<"\n";
    // std::cout<<"Final test:"<<cipher3.decrypt(cipher3.encrypt(input3))<<"\n";


    return 0;
}
 

