#pragma once

enum class TokenType {
    exit,
    mut,
    constant,
    int16,
    int32,
    int64,
    int_lit,
    boolean,
    identifier,
    colon,
    semi_colon,
    True,
    False,
    if_,
    else_,
    elif,
    while_,
    for_,
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

struct Token {
    TokenType type{};
    int line_no{};
    int column_no{};
    std::optional<std::string> value{};
};

inline std::string get_token(Token token ) {
    TokenType type = token.type;
    std::string token_name;
    switch(type) {
        case TokenType::exit:
            token_name = "exit";
            break;
        case TokenType::mut:
            token_name = "mut";
            break;
        case TokenType::constant:
            token_name = "const";
            break;
        case TokenType::int16:
            token_name = "int16";
            break;
        case TokenType::int32:
            token_name = "int32";
            break;
        case TokenType::int64:
            token_name = "int64";
            break;
        case TokenType::int_lit:
            token_name = token.value.value();
            break;
        case TokenType::boolean:
            token_name = "bool";
            break;
        case TokenType::identifier:
            token_name = token.value.value();
            break;
        case TokenType::colon:
            token_name = ":";
            break;
        case TokenType::semi_colon:
            token_name = ";";
            break;
        case TokenType::True:
            token_name = "true";
            break;
        case TokenType::False:
            token_name = "false";
            break;
        case TokenType::if_:
            token_name = "if";
            break;
        case TokenType::else_:
            token_name = "else";
            break;
        case TokenType::elif:
            token_name = "elif";
            break;
        case TokenType::while_:
            token_name = "while";
            break;
        case TokenType::for_:
            token_name = "for";
            break;
        case TokenType::plus:
            token_name = "+";
            break;
        case TokenType::minus:
            token_name = "-";
            break;
        case TokenType::equals:
            token_name = "=";
            break;
        case TokenType::star:
            token_name = "*";
            break;
        case TokenType::modulus:
            token_name = "%";
            break;
        case TokenType::pipe:
            token_name = "|";
            break;
        case TokenType::tilde:
            token_name = "~";
            break;
        case TokenType::caret:
            token_name = "^";
            break;
        case TokenType::ampersand:
            token_name = "&";
            break;
        case TokenType::postfix_add:
            token_name = "++";
            break;
        case TokenType::postfix_sub:
            token_name = "--";
            break;
        case TokenType::prefix_add:
            token_name = "++";
            break;
        case TokenType::prefix_sub:
            token_name = "--";
            break;
        case TokenType::forward_slash:
            token_name = "/";
            break;
        case TokenType::backward_slash:
            token_name = "\\";
            break;
        case TokenType::open_parenthesis:
            token_name = "(";
            break;
        case TokenType::close_parenthesis:
            token_name = ")";
            break;
        case TokenType::open_curly_bracket:
            token_name = "{";
            break;
        case TokenType::close_curly_bracket:
            token_name = "}";
            break;
        case TokenType::open_square_bracket:
            token_name = "[";
            break;
        case TokenType::close_square_bracket:
            token_name = "]";
            break;
    }
    return token_name;
}

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

bool is_statement(TokenType type) {
    switch(type) {
        case TokenType::if_:
        case TokenType::elif:
        case TokenType::else_:
        case TokenType::mut:
        case TokenType::constant:
        case TokenType::identifier:
        case TokenType::semi_colon:
            return true;;

        default:
            return false;
    }
}
