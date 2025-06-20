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
    PasswordManager& pm1 = PasswordManager::getInstance();

    std::cout<<"--------Password manager started! Please input your commands! ----------\n";
    CommandProcessor processor(pm1); // Pass the reference
    processor.run();

    return 0;
}
 

