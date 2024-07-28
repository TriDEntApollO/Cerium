#pragma once

#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <variant>
#include <optional>

#include "error.hpp"
#include "tokenize.hpp"
#include "varaibles.hpp"
#include "arenaallocator.hpp"


namespace Node {
    struct Expression;
    struct Statement;
    struct StmtIfNext;

    struct TermInt {
        Token int_lit;
    };

    struct TermIdent {
        Token identifier;
    };

    struct TermExpr {
        Expression *expr;
    };

    struct Term {
        std::variant<TermInt*, TermIdent*, TermExpr*> var;
    };

    struct BinAdd {
        Expression *left_side;
        Expression *right_side;
    };

    struct BinSubtract {
        Expression *left_side;
        Expression *right_side;
    };

    struct BinMultiply {
        Expression *left_side;
        Expression *right_side;
    };

    struct BinDivide {
        Expression *left_side;
        Expression *right_side;
    };

    struct BinModulus {
        Expression *left_side;
        Expression *right_side;
    };

    struct BitAnd{
        Expression *left_side;
        Expression *right_side;
    };

    struct BitOr {
        Expression *left_side;
        Expression *right_side;
    };

    struct BitXor {
        Expression *left_side;
        Expression *right_side;
    };

    struct BitNot {
        Expression *left_side;
        Expression *right_side;
    };

    struct BinExpr {
        std::variant<BinAdd*, BinSubtract*, BinMultiply*, BinDivide*, BinModulus*, BitAnd*, BitOr*, BitXor*> bin_expr;
    };

    struct Expression {
        std::variant<Term*, BinExpr*> var;
    };

    struct StmtExit {
        Expression *expr;
    };

    struct StmtMut {
        Token identifier;
        std::optional<Expression*> expr{};
    };

    struct StmtIdent {
        Token identifier;
        Expression *expr{};
    };

    struct Scope {
        std::vector<Statement*> stmts;
    };

    struct StmtElif {
        Expression *expr{};
        Scope *scope{};
        std::optional<StmtIfNext*> next;
    };

    struct StmtElse {
        Scope *scope{};
    };

    struct StmtIfNext {
        std::variant<StmtElif*, StmtElse*> var;
    };

    struct StmtIf {
        Expression *expr{};
        Scope *scope{};
        std::optional<StmtIfNext*> next;
    };


    struct Statement {
        std::variant<StmtExit*, StmtMut*, StmtIdent*, Scope*, StmtIf*> var;
    };

    struct Program {
        std::vector<Statement*> statements;
    };
}

class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens, std::string  filename) : m_tokens(std::move(tokens))
                                                                             , m_allocator(1024 * 1024 * 4)
                                                                             , m_filename(std::move(filename)) {
            m_curr_index = 0;
            m_current_scope = 0;
        }

        std::optional<Node::Term*> parse_term() {
            if (auto int_lit = try_grab(TokenType::int_lit)) {
                auto int_term = m_allocator.alloc<Node::TermInt>();
                int_term->int_lit = int_lit.value();
                auto term = m_allocator.alloc<Node::Term>();
                term->var = int_term;
                return term;
            }

            else if (auto identifier = try_grab(TokenType::identifier)) {
                auto identifier_term = m_allocator.alloc<Node::TermIdent>();
                if (m_variables.exists(identifier.value().value.value(), m_current_scope)) {
                    identifier_term->identifier = identifier.value();
                }
                else {
                    error_identifier(m_filename, "identifier not declared in this scope", identifier.value());
                }
                auto term = m_allocator.alloc<Node::Term>();
                term->var = identifier_term;
                return term;
            }

            else if (auto open_paren = try_grab(TokenType::open_parenthesis)) {
                if (auto expression = parse_expression()) {
                    try_grab(TokenType::close_parenthesis, "expected ')' ");
                    auto expression_term = m_allocator.alloc<Node::TermExpr>();
                    expression_term->expr = expression.value();
                    auto term = m_allocator.alloc<Node::Term>();
                    term->var = expression_term;
                    return term;
                }
                else {
                    return {};
                }
            }

            return {};
        }

        std::optional<Node::BinExpr*> parse_binary_expression(Node::Expression *expression, TokenType type, int precedence) {
            if (auto right_expression = parse_expression(precedence)) {
                if (type == TokenType::plus) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinAdd>();
                    left_expression->var = expression->var;
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
                else if (type == TokenType::minus) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinSubtract>();
                    left_expression->var = expression->var;
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
                else if (type == TokenType::star) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinMultiply>();
                    left_expression->var = expression->var;
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
                else if (type == TokenType::forward_slash) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinDivide>();
                    left_expression->var = expression->var;
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
                else if (type == TokenType::modulus) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_modulus = m_allocator.alloc<Node::BinModulus>();
                    left_expression->var = expression->var;
                    binary_modulus->left_side = left_expression;
                    binary_modulus->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_modulus;
                    return binary_expression;
                }
                else if (type == TokenType::ampersand) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto bitwise_and = m_allocator.alloc<Node::BitAnd>();
                    left_expression->var = expression->var;
                    bitwise_and->left_side = left_expression;
                    bitwise_and->right_side = right_expression.value();
                    binary_expression->bin_expr = bitwise_and;
                    return binary_expression;
                }
                else if (type == TokenType::caret) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto bitwise_xor = m_allocator.alloc<Node::BitXor>();
                    left_expression->var = expression->var;
                    bitwise_xor->left_side = left_expression;
                    bitwise_xor->right_side = right_expression.value();
                    binary_expression->bin_expr = bitwise_xor;
                    return binary_expression;
                }

                else if (type == TokenType::pipe) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto bitwise_or = m_allocator.alloc<Node::BitOr>();
                    left_expression->var = expression->var;
                    bitwise_or->left_side = left_expression;
                    bitwise_or->right_side = right_expression.value();
                    binary_expression->bin_expr = bitwise_or;
                    return binary_expression;
                }
            }

            return {};
        }

        std::optional<Node::Expression*> parse_expression(int minimum_precedence = 1) {
            if (auto term = parse_term()) {
                auto expression = m_allocator.alloc<Node::Expression>();
                expression->var = term.value();
                while (true) {
                    if (seek().has_value()) {
                        Token token = seek().value();
                        int precedence = operator_precedence(token.type);
                        if (precedence == 0 || precedence < minimum_precedence){
                            break;
                        }
                        grab();
                        int next_precedence = precedence + 1;
                        if (auto binary_expression = parse_binary_expression(expression, token.type, next_precedence)){
                            expression->var = binary_expression.value();
                        }
                        else {
                            return {};
                        }
                    }
                    else {
                        break;
                    }
                }

                return expression;
            }

            return {};
        }

        Node::Scope* parse_scope() {
            m_current_scope++;
            auto scope = m_allocator.alloc<Node::Scope>();
            while (auto stmt = parse_statement()) {
                scope->stmts.push_back(stmt.value());
            }

            try_grab(TokenType::close_curly_bracket, "expected '}'");
            m_current_scope--;

            return scope;
        }

        std::optional<Node::StmtIfNext*> parse_if_next() {
            if (auto token_elif = try_grab(TokenType::elif)) {
                try_grab(TokenType::open_parenthesis, "expected '('");
                auto elif_statement = m_allocator.alloc<Node::StmtElif>();

                if (auto expression = parse_expression()) {
                    elif_statement->expr = expression.value();
                }
                else {
                    if (auto token = seek()) {
                        error_expected(m_filename, "expected primary expression", token.value());
                    }
                    else {
                        error_expected(m_filename, "expected primary expression", "before the end of input", m_curr_line, m_curr_col);
                    }
                }

                try_grab(TokenType::close_parenthesis, "expected ')'");
                try_grab(TokenType::open_curly_bracket, "expected '{'");

                auto scope = parse_scope();
                elif_statement->scope = scope;

                if (auto next = parse_if_next()) {
                    elif_statement->next = next;
                }

                auto statement = m_allocator.alloc<Node::StmtIfNext>();
                statement->var = elif_statement;
                return statement;
            }

            else if (auto token_else = try_grab(TokenType::else_)) {
                try_grab(TokenType::open_curly_bracket, "expected '{'");
                auto else_statement = m_allocator.alloc<Node::StmtElse>();

                auto scope = parse_scope();
                else_statement->scope = scope;

                auto statement = m_allocator.alloc<Node::StmtIfNext>();
                statement->var = else_statement;
                return statement;
            }

            return {};
        }

        std::optional<Node::Statement*> parse_statement() {
            if (auto token_exit = try_grab(TokenType::exit)) {
                auto exit_statement = m_allocator.alloc<Node::StmtExit>();

                try_grab(TokenType::open_parenthesis, "expected '('");

                if (auto node_expr = parse_expression()) {
                    exit_statement->expr = node_expr.value();
                }
                else if (seek().has_value() && seek().value().type == TokenType::close_parenthesis) {
                    auto expression_int = m_allocator.alloc<Node::TermInt>();
                    expression_int->int_lit = { .type = TokenType::int_lit, .value = "0" };
                    auto term = m_allocator.alloc<Node::Term>();
                    term->var = expression_int;
                    auto exit_code = m_allocator.alloc<Node::Expression>();
                    exit_code->var = term;
                    exit_statement->expr = exit_code;
                }
                else {
                    if (auto token = seek()) {
                        error_expected(m_filename, "expected primary expression", token.value());
                    }
                    else {
                        error_expected(m_filename, "expected primary expression", "before the end of input", m_curr_line, m_curr_col);
                    }
                }

                try_grab(TokenType::close_parenthesis, "expected ')'");

                try_grab(TokenType::semi_colon, "expected ';'");

                auto statement = m_allocator.alloc<Node::Statement>();
                statement->var = exit_statement;
                return statement;
            }

            else if (auto token_mut = try_grab(TokenType::mut)) {
                auto mut_statement = m_allocator.alloc<Node::StmtMut>();
                auto identifier = try_grab(TokenType::identifier, "expected an identifier");

                if (identifier.has_value()) {
                    if (m_variables.is_valid(identifier.value().value.value(), m_current_scope)) {
                        mut_statement->identifier = identifier.value();
                        m_variables.declare_variable(identifier.value().value.value(), m_current_scope);

                        try_grab(TokenType::colon, "expected ':'");

                        if (identifier.has_value()) {
                            try_grab(TokenType::int64, "no type declaration for identifier '" + mut_statement->identifier.value.value() + "'");
                        }
                        else {
                            try_grab(TokenType::int64,"no type declaration");
                        }

                        if (auto equals = try_grab(TokenType::equals)) {
                            if (auto node_expr = parse_expression()) {
                                mut_statement->expr = node_expr.value();
                            } else {
                                if (auto token = seek()) {
                                    error_expected(m_filename, "expected primary expression", token.value());
                                } else {
                                    error_expected(m_filename, "expected primary expression", "before the end of input",
                                                   m_curr_line, m_curr_col);
                                }
                            }
                        }

                        if (seek().has_value() && (seek().value().type == TokenType::identifier || seek().value().type == TokenType::int_lit
                                                || seek().value().type == TokenType::open_parenthesis)) {
                            auto token = seek();
                            error_expected(m_filename, "expected '='", token.value());
                        }
                    }
                    else {
                        error_identifier(m_filename, "multiple definitions of identifier", identifier.value());
                        while(seek().has_value() && !is_statement(seek().value().type)) {
                            grab();
                        }
                    }
                }
                else {
                    while (seek().has_value() && !is_statement(seek().value().type)) {
                        grab();
                    }
                }

                try_grab(TokenType::semi_colon, "expected ';'");

                auto statement = m_allocator.alloc<Node::Statement>();
                statement->var = mut_statement;
                return statement;
            }

            else if (auto token_identifier = try_grab(TokenType::identifier)) {
                auto identifier_statement = m_allocator.alloc<Node::StmtIdent>();
                identifier_statement->identifier = token_identifier.value();
                if (m_variables.exists(token_identifier.value().value.value(), m_current_scope)) {
                    try_grab(TokenType::equals, "expected '='");

                    if (auto node_expr = parse_expression()) {
                        identifier_statement->expr = node_expr.value();
                    }
                    else {
                        if (auto token = seek()) {
                            error_expected(m_filename, "expected primary expression", token.value());
                        }
                        else {
                            error_expected(m_filename, "expected primary expression", "before the end of input", m_curr_line, m_curr_col);
                        }
                    }
                }
                else {
                    error_identifier(m_filename, "identifier not declared in this scope",token_identifier.value());
                    while(seek().has_value() && !is_statement(seek().value().type)) {
                        grab();
                    }
                }

                try_grab(TokenType::semi_colon, "expected ';'");

                auto statement = m_allocator.alloc<Node::Statement>();
                statement->var = identifier_statement;
                return statement;
            }

            else if (auto token_open_curly = try_grab(TokenType::open_curly_bracket)) {
                auto scope = parse_scope();
                auto statement = m_allocator.alloc<Node::Statement>();
                statement->var = scope;
                return statement;
            }

            else if (auto token_if = try_grab(TokenType::if_)) {
                try_grab(TokenType::open_parenthesis, "expected '('");
                auto if_statement = m_allocator.alloc<Node::StmtIf>();

                if (auto expression = parse_expression()) {
                    if_statement->expr = expression.value();
                }
                else {
                    if (auto token = seek()) {
                        error_expected(m_filename, "expected primary expression", token.value());
                    }
                    else {
                        error_expected(m_filename, "expected primary expression", "before the end of input", m_curr_line, m_curr_col);
                    }
                }

                try_grab(TokenType::close_parenthesis, "expected ')'");
                try_grab(TokenType::open_curly_bracket, "expected '{'");

                auto scope = parse_scope();
                if_statement->scope = scope;

                if (auto next = parse_if_next()) {
                    if_statement->next = next;
                }

                auto statement = m_allocator.alloc<Node::Statement>();
                statement->var = if_statement;
                return statement;

            }

            else if (auto token_semi = try_grab(TokenType::semi_colon)) {
                grab();
            }

            return {};
        }

        std::pair<Node::Program, Variables> parse_program() {
            Node::Program program;
            while(seek().has_value()) {
                if (auto statement = parse_statement()) {
                    program.statements.push_back(statement.value());
                }
                else {
                    std::cerr << "cer: error: invalid statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            std::pair<Node::Program, Variables> pair;
            pair.first = program;
            pair.second = m_variables;

            return pair;
        }

    private:
        int m_curr_col;
        int m_curr_line;
        int m_current_scope;
        Variables m_variables;
        size_t m_curr_index;
        const std::string m_filename;
        ArenaAllocator m_allocator;
        const std::vector<Token> m_tokens;

        [[nodiscard]] inline std::optional<Token> seek(int offset = 0) const {
            if (m_curr_index + offset >= m_tokens.size()) {
                return {};
            }
            return m_tokens.at(m_curr_index + offset);
        }

        inline std::optional<Token> try_grab(TokenType type, const std::string& error_msg) {
            if (seek().has_value() && seek().value().type == type) {
                return grab();
            }
            else {
                if (auto token = seek()) {
                    error_expected(m_filename, error_msg, token.value());
                }
                else {
                    error_expected(m_filename, error_msg, "before the end of input", m_curr_line, m_curr_col);
                }
            }

            return {};
        }

        inline std::optional<Token> try_grab(TokenType type) {
            if (seek().has_value() && seek().value().type == type) {
                return grab();
            }

            return {};
        }

        inline Token grab() {
            Token token = m_tokens.at(m_curr_index++);
            m_curr_line = token.line_no;
            m_curr_col = token.column_no;
            return token;
        }
};
