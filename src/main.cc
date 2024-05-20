/*
 * @Author: Wenyu Kui 
 * @Date: 2024-05-19 13:02:45 
 * @Last Modified by: Wenyu Kui
 * @Last Modified time: 2024-05-19 22:30:19
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "eprom_emulator.h"

auto vep = veprom::EpromEmulator();

std::vector<std::string> convertToTokens(int argc, char* argv[]) {
    std::vector<std::string> tokens;
    for (int i = 1; i < argc; ++i) {
        tokens.emplace_back(std::string(argv[i]));
    }
    return tokens;
}

/*
 * Check whether s is a number
 * return 1 if s is a number and save the number to val
 * return 0 if s is NOT a number
 */
int isNum(std::string& s, int& val) {
    char* p;
    int converted = strtol(s.c_str(), &p, 10);
    if (*p == 0) {
        val = converted;
        return 1;
    }
    return 0;
}
int isHex(std::string& s, int& val) {
    // hex starting from "0x"
    if (s.length() > 2 && s[0] == '0' && s[1] == 'x') {
        char* p;
        int converted = strtol(s.c_str(), &p, 16);
        if (*p == 0) {
            val = converted;
            return 1;
        }
    }
    return 0;
}

int commandWrapper(std::vector<std::string> tokens) {
    if (tokens.empty())
        return veprom::ERR_COMMAND;
    else if (tokens[0] == "create") {
        if (tokens.size() != 2)
            return veprom::ERR_COMMAND;
        // check whether tokens[1] is a number
        int val;
        if (isNum(tokens[1], val) && val < veprom::EPROM_SIZE_LIMIT) {
            return vep.create(val);
        } else {
            return veprom::ERR_COMMAND;
        }

    } else if (tokens[0] == "load") {
        if (tokens.size() != 2)
            return veprom::ERR_COMMAND;
        return vep.load(tokens[1]);

    } else if (tokens[0] == "write_raw") {
        if (tokens.size() != 3)
            return veprom::ERR_COMMAND;
        // check whether tokens[1]($ADDRESS) is valid
        int addr;
        if (isNum(tokens[1], addr) || isHex(tokens[1], addr)) {
            return vep.write_raw(addr, tokens[2]);
        } else {
            return veprom::ERR_COMMAND;
        }
    } else if (tokens[0] == "read_raw") {
        if (tokens.size() != 3)
            return veprom::ERR_COMMAND;
        int addr;
        int len;
        if ((isNum(tokens[1], addr) || isHex(tokens[1], addr)) &&
            (isNum(tokens[2], len))) {
            return vep.read_raw(addr, len);
        } else {
            return veprom::ERR_COMMAND;
        }
    } else if (tokens[0] == "list") {
        return vep.list();
    } else if (tokens[0] == "erase") {
        int ret = vep.erase();
        if (ret)
            return ret;
        return 0;
    } else if (tokens[0] == "write") {
        if (tokens.size() != 2)
            return veprom::ERR_COMMAND;
    } else if (tokens[0] == "read") {
        if (tokens.size() != 2)
            return veprom::ERR_COMMAND;
    } else {
        std::cerr << "Command not found!" << std::endl;
        return veprom::ERR_COMMAND;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <string>" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    std::vector<std::string> tokens = convertToTokens(argc, argv);

    std::cout << "\n>>> COMMAND:";
    for (const auto& token : tokens) {
        std::cout << " " << token;
    }
    std::cout << std::endl;
    commandWrapper(tokens);

    return 0;
}