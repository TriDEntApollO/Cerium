#pragma once

#include <utility>

#include "token.hpp"

bool error_flag = false;


inline void error_identifier(const std::string &filename, std::string message, Token identifier) {
    error_flag = true;
    int line = identifier.line_no;
    int col = identifier.column_no;
    std::string name = get_token(std::move(identifier));
    std::cerr << "cer: error: " << message << std::endl;
    std::cerr << filename << "::" << line << ":" << col << ": identifier '" << name << "'" << std::endl;
    std::cerr << std::endl;
}

inline void error_expected(const std::string &filename, const std::string &message, Token &token) {
    error_flag = true;
    int line = token.line_no;
    int col = token.column_no;
    std::string token_name = get_token(std::move(token));
    std::cerr << "cer: error: " << message << std::endl;
    std::cerr << filename << "::" << line << ":" << col << ": before token '" << token_name << "'" << std::endl;
    std::cerr << std::endl;
}

inline void error_expected(const std::string &filename, const std::string &message, const std::string token, int line, int col) {
    error_flag = true;
    std::cerr << "cer: error: " << message << std::endl;
    std::cerr << filename << "::" << line << ":" << col << ": " << token << "" << std::endl;
    std::cerr << std::endl;
}