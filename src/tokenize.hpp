#pragma once

#include <string>
#include <vector>
#include <optional>

#include "error.hpp"
#include "token.hpp"


class Tokenizer {
    public:
        inline explicit Tokenizer(std::string src_code, std::string filename) : m_src_code(std::move(src_code))
                                                                              , m_filename(std::move(filename)) {
            m_curr_index = 0;
            line_count = 1;
            column_count = 1;
        }

        inline std::vector<Token> tokenize() {
            std::string buff;
            std::vector<Token> tokens;
            while (seek().has_value()) {
                if (std::isalpha(seek().value())) {
                    int col = column_count;
//                    buff.push_back(grab());
                    while (seek().has_value() && std::isalnum(seek().value())){
                        buff.push_back(grab());
                        column_count++;
                    }

                    if (buff == "exit") {
                        tokens.push_back({ .type = TokenType::exit, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "mut") {
                        tokens.push_back({ .type = TokenType::mut, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "int64") {
                        tokens.push_back({ .type = TokenType::int64, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "bool") {
                        tokens.push_back({ .type = TokenType::boolean, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "true") {
                        tokens.push_back({ .type = TokenType::True, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "false") {
                        tokens.push_back({ .type = TokenType::False, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "if") {
                        tokens.push_back({ .type = TokenType::if_, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "else") {
                        tokens.push_back({ .type = TokenType::else_, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else if (buff == "elif") {
                        tokens.push_back({ .type = TokenType::elif, .line_no = line_count, .column_no = col });
                        buff.clear();
                    }
                    else {
                        tokens.push_back({ .type = TokenType::identifier, .line_no = line_count, .column_no = col, .value = buff });
                        buff.clear();
                    }
                }

                else if (std::isdigit(seek().value())) {
                    int col = column_count;
//                    buff.push_back(grab());
                    while (seek().has_value() && std::isdigit(seek().value())) {
                        buff.push_back(grab());
                        column_count++;
                    }

                    tokens.push_back({ .type = TokenType::int_lit, .line_no = line_count, .column_no = col, .value = buff });
                    buff.clear();
                }

                else {
                    switch (seek().value()) {
                        case '=':
                            grab();
                            tokens.push_back({ .type = TokenType::equals, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '+':
                            grab();
                            tokens.push_back({ .type = TokenType::plus, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '-':
                            grab();
                            tokens.push_back({ .type = TokenType::minus, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '*':
                            grab();
                            tokens.push_back({ .type = TokenType::star, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '/':
                            grab();
                            if (seek().has_value() && seek().value() == '/') {
                                grab();
                                while(seek().has_value() && seek().value() != '\n') {
                                    grab();
                                }
                            }
                            else if (seek().has_value() && seek().value() == '*') {
                                grab();
                                int line = line_count, col = column_count;
                                bool not_terminated = true;
                                while(seek().has_value()) {
                                    if (seek().value() == '\n') {
                                        line_count++;
                                    }
                                    if (grab() == '*' && seek().has_value() && seek().value() == '/') {
                                        grab();
                                        not_terminated = false;
                                        break;
                                    }
                                }

                                if (not_terminated) {
                                    error_expected(m_filename, "unterminated comment", "/*", line, col);
                                }
                            }
                            else {
                                tokens.push_back({ .type = TokenType::forward_slash, .line_no = line_count, .column_no = column_count });
                                column_count++;
                            }
                            break;

                        case '%':
                            grab();
                            tokens.push_back({ .type = TokenType::modulus, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '\\':
                            grab();
                            tokens.push_back({ .type = TokenType::backward_slash, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '^':
                            grab();
                            tokens.push_back({ .type = TokenType::caret, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '~':
                            grab();
                            tokens.push_back({ .type = TokenType::tilde, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '|':
                            grab();
                            tokens.push_back({ .type = TokenType::pipe, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '&':
                            grab();
                            tokens.push_back({ .type = TokenType::ampersand, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '(':
                            grab();
                            tokens.push_back({ .type = TokenType::open_parenthesis, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case ')':
                            grab();
                            tokens.push_back({ .type = TokenType::close_parenthesis, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '{':
                            grab();
                            tokens.push_back({ .type = TokenType::open_curly_bracket, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '}':
                            grab();
                            tokens.push_back({ .type = TokenType::close_curly_bracket, .line_no = line_count, .column_no = column_count});
                            column_count++;
                            break;

                        case '[':
                            grab();
                            tokens.push_back({ .type = TokenType::open_square_bracket, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case ']':
                            grab();
                            tokens.push_back({ .type = TokenType::close_square_bracket, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case ':':
                            grab();
                            tokens.push_back({ .type = TokenType::colon, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case ';':
                            grab();
                            tokens.push_back({ .type = TokenType::semi_colon, .line_no = line_count, .column_no = column_count });
                            column_count++;
                            break;

                        case '\n':
                            grab();
                            column_count = 1;
                            line_count++;
                            break;

                        case ' ':
                        case '\t':
                            column_count++;
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
        int line_count;
        int column_count;
        size_t m_curr_index;
        const std::string m_src_code;
        const std::string m_filename;

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
