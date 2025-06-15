#pragma once
#include "passwordManager.hpp"
#include <string>
#include <sstream>
#include <iostream>

class CommandProcessor {
public:
    CommandProcessor(PasswordManager& manager) : manager(manager) {}

    void run();
    void process(const std::string& line);

private:
    PasswordManager& manager;
};
