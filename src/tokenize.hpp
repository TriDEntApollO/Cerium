#pragma once

#include <string>
#include <vector>
#include <optional>

enum class TokenType {
    exit,
    int16,
    int32,
    int64,
    int_lit,
    constant,
    identifier,
    colon,
    semi_colon,
    plus,
    minus,
    equals,
    star,
    modulus,
    pipe,
    tilde,
    caret,
    ampersand,
    postfix_add,
    postfix_sub,
    prefix_add,
    prefix_sub,
    forward_slash,
    backward_slash,
    open_parenthesis,
    close_parenthesis,
    open_curly_bracket,
    close_curly_bracket,
    open_square_bracket,
    close_square_bracket,
};

int operator_precedence(TokenType type) {
    switch(type) {
        case TokenType::pipe:

            return 1;
        case TokenType::caret:

            return 2;
        case TokenType::ampersand:

            return 3;

        case TokenType::plus:
        case TokenType::minus:
            return 4;

        case TokenType::star:
        case TokenType::forward_slash:
        case TokenType::modulus:
            return 5;

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

                else {
                    switch (seek().value()) {
                        case '=':
                            grab();
                            tokens.push_back({ .type = TokenType::equals });
                            break;

                        case '+':
                            grab();
                            tokens.push_back({ .type = TokenType::plus });
                            break;

                        case '-':
                            grab();
                            tokens.push_back({ .type = TokenType::minus });
                            break;

                        case '*':
                            grab();
                            tokens.push_back({ .type = TokenType::star });
                            break;

                        case '/':
                            grab();
                            tokens.push_back({ .type = TokenType::forward_slash });
                            break;

                        case '%':
                            grab();
                            tokens.push_back({ .type = TokenType::modulus });
                            break;

                        case '\\':
                            grab();
                            tokens.push_back({ .type = TokenType::backward_slash });
                            break;

                        case '^':
                            grab();
                            tokens.push_back({ .type = TokenType::caret });
                            break;

                        case '~':
                            grab();
                            tokens.push_back({ .type = TokenType::tilde });
                            break;

                        case '|':
                            grab();
                            tokens.push_back({ .type = TokenType::pipe });
                            break;

                        case '&':
                            grab();
                            tokens.push_back({ .type = TokenType::ampersand });
                            break;

                        case '(':
                            grab();
                            tokens.push_back({ .type = TokenType::open_parenthesis });
                            break;

                        case ')':
                            grab();
                            tokens.push_back({ .type = TokenType::close_parenthesis });
                            break;

                        case '{':
                            grab();
                            tokens.push_back({ .type = TokenType::open_curly_bracket });
                            break;

                        case '}':
                            grab();
                            tokens.push_back({ .type = TokenType::close_curly_bracket});
                            break;

                        case '[':
                            grab();
                            tokens.push_back({ .type = TokenType::open_square_bracket });
                            break;

                        case ']':
                            grab();
                            tokens.push_back({ .type = TokenType::close_square_bracket });
                            break;

                        case ':':
                            grab();
                            tokens.push_back({ .type = TokenType::colon });
                            break;

                        case ';':
                            grab();
                            tokens.push_back({ .type = TokenType::semi_colon });
                            break;

                        case ' ':
                        case '\t':
                        case '\n':
                            grab();
                            break;

                        default:
                            std::cerr << "cer: error: invalid token" << std::endl;
                            exit(EXIT_FAILURE);
                    }
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
