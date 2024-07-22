#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <variant>
#include <optional>

#include "tokenize.hpp"
#include "arenaallocator.hpp"


namespace Node {
    struct BinExpr;

    struct TermInt {
        Token int_lit;
    };

    struct TermIdent {
        Token identifier;
    };

    struct Term {
        std::variant<TermInt*, TermIdent*> var;
    };

    struct Expression {
        std::variant<Term*, BinExpr*> var;
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

    struct BinExpr {
        std::variant<BinAdd*, BinSubtract*, BinMultiply*, BinDivide*> bin_expr;
    };

    struct StmtExit {
        Expression *expr;
    };

    struct StmtConst {
        Token identifier;
        Expression *expr{};
    };

    struct Statement {
        std::variant<StmtExit*, StmtConst*> var;
    };

    struct Program {
        std::vector<Statement*> statements;
    };
}

class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 4) {
            m_curr_index = 0;
        }

        std::optional<Node::Term*> parse_term() {
            if (seek().has_value() && seek().value().type == TokenType::int_lit) {
                auto expression_int = m_allocator.alloc<Node::TermInt>();
                expression_int->int_lit = grab();
                auto term = m_allocator.alloc<Node::Term>();
                term->var = expression_int;
                return term;
            }

            else if (seek().has_value() && seek().value().type == TokenType::identifier) {
                auto expression_identifier = m_allocator.alloc<Node::TermIdent>();
                expression_identifier->identifier = grab();
                auto term = m_allocator.alloc<Node::Term>();
                term->var = expression_identifier;
                return term;
            }

            return {};
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
        std::optional<Node::BinExpr*> parse_binary_expression(Node::Expression *left_expression) {
            TokenType type = grab().type;
            if (auto right_expression = parse_expression()) {
                if (type == TokenType::add) {
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinAdd>();
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
                else if (type == TokenType::sub) {
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinSubtract>();
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
                else if (type == TokenType::mul) {
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinMultiply>();
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
                else if (type == TokenType::div) {
                    auto binary_expression = m_allocator.alloc<Node::BinExpr>();
                    auto binary_add = m_allocator.alloc<Node::BinDivide>();
                    binary_add->left_side = left_expression;
                    binary_add->right_side = right_expression.value();
                    binary_expression->bin_expr = binary_add;
                    return binary_expression;
                }
            }

            return {};
        }
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
        std::optional<Node::Expression*> parse_expression() {
            if (auto term = parse_term()) {
                if (seek().has_value() && seek().value().type == TokenType::add) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    left_expression->var = term.value();
                    if (auto binary_expression = parse_binary_expression(left_expression)) {
                        auto expression = m_allocator.alloc<Node::Expression>();
                        expression->var = binary_expression.value();
                        return expression;
                    }
                }
                else if (seek().has_value() && seek().value().type == TokenType::sub) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    left_expression->var = term.value();
                    if (auto binary_expression = parse_binary_expression(left_expression)) {
                        auto expression = m_allocator.alloc<Node::Expression>();
                        expression->var = binary_expression.value();
                        return expression;
                    }
                }
                else if (seek().has_value() && seek().value().type == TokenType::mul) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    left_expression->var = term.value();
                    if (auto binary_expression = parse_binary_expression(left_expression)) {
                        auto expression = m_allocator.alloc<Node::Expression>();
                        expression->var = binary_expression.value();
                        return expression;
                    }
                }
                else if (seek().has_value() && seek().value().type == TokenType::div) {
                    auto left_expression = m_allocator.alloc<Node::Expression>();
                    left_expression->var = term.value();
                    if (auto binary_expression = parse_binary_expression(left_expression)) {
                        auto expression = m_allocator.alloc<Node::Expression>();
                        expression->var = binary_expression.value();
                        return expression;
                    }
                }
                else {
                    auto expression = m_allocator.alloc<Node::Expression>();
                    expression->var = term.value();
                    return expression;
                }
            }

            return {};
        }
#pragma clang diagnostic pop

        std::optional<Node::Statement*> parse_statement() {
            if (seek().has_value() && seek().value().type == TokenType::exit) {
                grab();
                auto exit_statement = m_allocator.alloc<Node::StmtExit>();

                if (seek().has_value() && seek().value().type == TokenType::open_parenthesis) {
                    grab();
                }
                else {
                    std::cerr << "cer: error: expected '('" << std::endl;
                    exit(EXIT_FAILURE);
                }

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
                    std::cerr << "cer: error: invalid expression: exit" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (seek().has_value() && seek().value().type == TokenType::close_parenthesis) {
                    grab();
                }
                else {
                    std::cerr << "cer: error: expected ')'" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (seek().has_value() && seek().value().type == TokenType::semi) {
                    grab();
                }
                else {
                    std::cerr << "cer: error: Expected ';'" << std::endl;
                    exit(EXIT_FAILURE);
                }

                auto statement = m_allocator.alloc<Node::Statement>();
                statement->var = exit_statement;
                return statement;
            }

            else if (seek().has_value() && seek().value().type == TokenType::constant) {
                grab();
                auto const_statement = m_allocator.alloc<Node::StmtConst>();

                if (seek().has_value() && seek().value().type == TokenType::identifier) {
                    const_statement->identifier = grab() ;
                }
                else {
                    std::cerr << "cer: error: expected an identifier" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (seek().has_value() && seek().value().type == TokenType::colon) {
                    grab();
                }
                else {
                    std::cerr << "cer: error: expected ':'" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (seek().has_value() && seek().value().type == TokenType::int64) {
                    grab();
                }
                else {
                    std::cerr << "cer: error: no declaration of type for identifier '" << const_statement->identifier.value.value() << "'" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (seek().has_value() && seek().value().type == TokenType::equals) {
                    grab();
                }
                else {
                    std::cerr << "cer: error: expected '='" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (auto node_expr = parse_expression()) {
                    const_statement->expr = node_expr.value();
                }
                else {
                    std::cerr << "cer: error: invalid expression: const" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (seek().has_value() && seek().value().type == TokenType::semi) {
                    grab();
                }
                else {
                    std::cerr << "cer: error: Expected ';'" << std::endl;
                    exit(EXIT_FAILURE);
                }

                auto statement = m_allocator.alloc<Node::Statement>();
                statement->var = const_statement;
                return statement;
            }

            return {};
        }

        std::optional<Node::Program> parse_program() {
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

            return program;
        }

    private:
        const std::vector<Token> m_tokens;
        size_t m_curr_index;
        ArenaAllocator m_allocator;

        [[nodiscard]] inline std::optional<Token> seek(int offset = 0) const {
            if (m_curr_index + offset >= m_tokens.size()) {
                return {};
            }
            return m_tokens.at(m_curr_index + offset);
        }


        inline Token grab() {
            return (m_tokens.at(m_curr_index++));
        }
};
