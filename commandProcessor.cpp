#include "commandProcessor.hpp"

void CommandProcessor::run() {
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            process(line);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }
}

void CommandProcessor::process(const std::string& line) {
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    if (cmd == "create") {
        std::string name, cipher, pass, args;
        iss >> name >> cipher >> pass;
        std::getline(iss, args); // optional args
        manager.create(name, cipher, pass, args);
    } else if (cmd == "open") {
        std::string name, pass;
        iss >> name >> pass;
        manager.open(name, pass);
    } else if (cmd == "save") {
        std::string site, user, pass;
        iss >> site >> user >> pass;
        manager.save(site, user, pass);
    } else if (cmd == "load") {
        std::string site, user;
        iss >> site >> user;
        manager.load(site, user);
    } else if (cmd == "update") {
        std::string site, user, newPass;
        iss >> site >> user >> newPass;
        manager.update(site, user, newPass);
    } else if (cmd == "delete") {
        std::string site, user;
        iss >> site >> user;
        manager.del(site, user);
    } else if (cmd == "exit" || cmd == "quit") {
        std::exit(0);
    } else {
        throw std::runtime_error("Unknown command: " + cmd);
    }
}