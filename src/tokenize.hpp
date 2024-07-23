#pragma once

#include <string>
#include <vector>
#include <optional>

enum class TokenType {
    exit,
    int16,
    int32,
    int64,
    plus,
    minus,
    star,
    forward_slash,
    colon,
    equals,
    int_lit,
    constant,
    identifier,
    open_parenthesis,
    close_parenthesis,
    semi
};

int operator_precedence(TokenType type) {
    switch(type) {
        case TokenType::plus:
        case TokenType::minus:
            return 1;

        case TokenType::star:
        case TokenType::forward_slash:
            return 2;

        default:
            return 0;
    }
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
    public:
        inline explicit Tokenizer(std::string src_code) : m_src_code(std::move(src_code)) {
            m_curr_index = 0;
        }

        inline std::vector<Token> tokenize() {
            std::string buff;
            std::vector<Token> tokens;
            while (seek().has_value()) {
                if (std::isalpha(seek().value())) {
                    buff.push_back(grab());
                    while (seek().has_value() && std::isalnum(seek().value())){
                        buff.push_back(grab());
                    }

                    if (buff == "exit") {
                        tokens.push_back({ .type = TokenType::exit });
                        buff.clear();
                    }
                    else if (buff == "const") {
                        tokens.push_back({ .type = TokenType::constant });
                        buff.clear();
                    }
                    else if (buff == "int64") {
                        tokens.push_back({ .type = TokenType::int64 });
                        buff.clear();
                    }
                    else {
                        tokens.push_back({ .type = TokenType::identifier, .value = buff });
                        buff.clear();
                    }
                }

                else if (std::isdigit(seek().value())) {
                    buff.push_back(grab());
                    while (seek().has_value() && std::isdigit(seek().value())) {
                        buff.push_back(grab());
                    }

                    tokens.push_back({ .type = TokenType::int_lit, .value = buff });
                    buff.clear();
                }

                else if (seek().value() == '=') {
                    grab();
                    tokens.push_back({ .type = TokenType::equals });
                }

                else if (seek().value() == '+') {
                    grab();
                    tokens.push_back({ .type = TokenType::plus });
                }

                else if (seek().value() == '-') {
                    grab();
                    tokens.push_back({ .type = TokenType::minus });
                }

                else if (seek().value() == '*') {
                    grab();
                    tokens.push_back({ .type = TokenType::star });
                }

                else if (seek().value() == '/') {
                    grab();
                    tokens.push_back({ .type = TokenType::forward_slash });
                }

                else if (seek().value() == '(') {
                    grab();
                    tokens.push_back({ .type = TokenType::open_parenthesis });
                }

                else if (seek().value() == ')') {
                    grab();
                    tokens.push_back({ .type = TokenType::close_parenthesis });
                }

                else if (seek().value() == ':') {
                    grab();
                    tokens.push_back({ .type = TokenType::colon });
                }


                else if (seek().value() == ';') {
                    grab();
                    tokens.push_back({ .type = TokenType::semi });
                }

                else if (seek().value() == ' ' || seek().value() == '\t' || seek().value() == '\n' || std::isspace(seek().value())) {
                    grab();
                }

                else {
                    std::cerr << "cer: error: invalid token" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

        m_curr_index = 0;
        return tokens;
    }

    private:
        const std::string m_src_code;
        size_t m_curr_index;

        [[nodiscard]] inline std::optional<char> seek(int offset = 0) const {
            if (m_curr_index + offset >= m_src_code.length()){
                return {};
            }
            return m_src_code.at(m_curr_index + offset);
        }

        inline char grab() {
            return m_src_code.at(m_curr_index++);
        }
};
