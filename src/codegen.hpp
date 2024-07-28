#pragma once

#include <map>
#include <stack>
#include <vector>
#include <string>
#include <sstream>

#include "error.hpp"
#include "parser.hpp"
#include "varaibles.hpp"


class CodeGenerator {
public:
    inline explicit CodeGenerator(Node::Program program_node, const Variables& variables) : m_program_node(std::move(program_node))
                                                                                    , m_variables(variables){
        m_stack_pointer = 0;
        m_current_scope = 0;
        m_label_map = { {TokenType::if_, 0}, {TokenType::else_, 0},
                        {TokenType::elif, 0}, {TokenType::while_, 0},
                        {TokenType::for_, 0}, {TokenType::exit, 0}};
    }

    [[nodiscard]] std::string generate_term(const Node::Term *term) {
        struct ExpressionVisitor {
            std::stringstream &code_stream;
            CodeGenerator &generator;

            void operator() (const Node::TermInt *int_term) const {
                code_stream << "\tmov rax, " << int_term->int_lit.value.value() << "\n";
                code_stream << generator.push_stack("rax");
            }

            void operator() (const Node::TermIdent *identifier_term) {
                std::string variable_identifier = identifier_term->identifier.value.value();
                auto iterator = generator.m_variables.get_variable(variable_identifier, generator.m_current_scope);
                    const Variable& variable = iterator->second;
                    std::stringstream offset;
                    offset << "QWORD [rsp + " << (generator.m_stack_pointer - variable.stack_location) * 8 << "]";
                    code_stream << generator.push_stack(offset.str());
            }

            void operator() (const Node::TermExpr *expression_term) const {
                code_stream << generator.generate_expression(expression_term->expr);
            }
        };
        std::stringstream code;
        ExpressionVisitor visitor{.code_stream = code, .generator = *this};
        std::visit(visitor, term->var);

        return code.str();
    }

    [[nodiscard]] std::string generate_binary_expression(const Node::BinExpr *binary_expression) {
        struct BinExpressionVisitor {
            std::stringstream &code_stream;
            CodeGenerator &generator;

            void operator() (const Node::BinAdd *binary_add) const {
                code_stream << generator.generate_expression(binary_add->left_side);
                code_stream << generator.generate_expression(binary_add->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\tadd rax, rbx\n";
                code_stream << generator.push_stack("rax");
            }

            void operator() (const Node::BinSubtract *binary_subtract) const {
                code_stream << generator.generate_expression(binary_subtract->left_side);
                code_stream << generator.generate_expression(binary_subtract->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\tsub rax, rbx\n";
                code_stream << generator.push_stack("rax");
            }

            void operator() (const Node::BinMultiply *binary_multiply) const {
                code_stream << generator.generate_expression(binary_multiply->left_side);
                code_stream << generator.generate_expression(binary_multiply->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\tmul rbx\n";
                code_stream << generator.push_stack("rax");
            }

            void operator() (const Node::BinDivide *binary_divide) const {
                code_stream << generator.generate_expression(binary_divide->left_side);
                code_stream << generator.generate_expression(binary_divide->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\tdiv rbx\n";
                code_stream << generator.push_stack("rax");
            }

            void operator() (const Node::BinModulus *binary_modulus) const {
                code_stream << generator.generate_expression(binary_modulus->left_side);
                code_stream << generator.generate_expression(binary_modulus->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\tdiv rbx\n";
                code_stream << generator.push_stack("rdx");
            }

            void operator() (const Node::BitAnd *bitwise_and) const {
                code_stream << generator.generate_expression(bitwise_and->left_side);
                code_stream << generator.generate_expression(bitwise_and->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\tand rax, rbx\n";
                code_stream << generator.push_stack("rax");
            }

            void operator() (const Node::BitXor *bitwise_xor) const {
                code_stream << generator.generate_expression(bitwise_xor->left_side);
                code_stream << generator.generate_expression(bitwise_xor->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\txor rax, rbx\n";
                code_stream << generator.push_stack("rax");
            }

            void operator() (const Node::BitOr *bitwise_or) const {
                code_stream << generator.generate_expression(bitwise_or->left_side);
                code_stream << generator.generate_expression(bitwise_or->right_side);
                code_stream << generator.pop_stack("rbx");
                code_stream << generator.pop_stack("rax");
                code_stream << "\tor rax, rbx\n";
                code_stream << generator.push_stack("rax");
            }
        };
        std::stringstream code;
        BinExpressionVisitor visitor{.code_stream = code, .generator = *this};
        std::visit(visitor, binary_expression->bin_expr);

        return code.str();
    }

    [[nodiscard]] std::string generate_expression(const Node::Expression *expression) {
        struct ExpressionVisitor {
            std::stringstream &code_stream;
            CodeGenerator &generator;

            void operator() (const Node::Term *term) const {
                code_stream << generator.generate_term(term);
            }

            void operator() (const Node::BinExpr *binary_expression) {
                code_stream << generator.generate_binary_expression(binary_expression);
            }
        };
        std::stringstream code;
        ExpressionVisitor visitor{.code_stream = code, .generator = *this};
        std::visit(visitor, expression->var);

        return code.str();
    }

    [[nodiscard]] std::string generate_if_next(const Node::StmtIfNext *if_next) {
        struct ExpressionVisitor {
            std::stringstream &code_stream;
            CodeGenerator &generator;

            void operator() (const Node::StmtElif *elif_statement) const {
                std::stringstream buffer;
                std::string elif_label = generator.generate_label(TokenType::elif);
                generator.m_labels.push_back(elif_label);
                code_stream << "\n" << elif_label << ":\n";
                code_stream << generator.generate_expression(elif_statement->expr);
                code_stream << generator.pop_stack("rax");
                code_stream << "\ttest rax, rax\n";
                generator.begin_scope();
                for (const Node::Statement *stmt : elif_statement->scope->stmts) {
                    buffer << generator.generate_statement(stmt);
                }
                buffer << generator.end_scope();
                buffer << "\tjmp " << generator.m_labels.front() << "\n";
                if (elif_statement->next.has_value()) {
                    buffer << generator.generate_if_next(elif_statement->next.value());
                    code_stream << "\tjz " << generator.m_labels.back() << "\n";
                    generator.m_labels.pop_back();
                }
                else {
                    code_stream << "\tjz " << generator.m_labels.front() << "\n";
                }
                code_stream << buffer.str();
            }

            void operator() (const Node::StmtElse *else_statement) {
                std::string else_label = generator.generate_label(TokenType::else_);
                generator.m_labels.push_back(else_label);
                code_stream << "\n" << else_label << ":\n";
                generator.begin_scope();
                for (const Node::Statement *stmt : else_statement->scope->stmts) {
                    code_stream << generator.generate_statement(stmt);
                }
                code_stream << generator.end_scope();
                code_stream << "jmp " << generator.m_labels.front() << "\n";
            }
        };
        std::stringstream code;
        ExpressionVisitor visitor{.code_stream = code, .generator = *this};
        std::visit(visitor, if_next->var);

        return code.str();
    }

    [[nodiscard]] std::string generate_statement(const Node::Statement *statement) {
        struct StatementVisitor {
            std::stringstream &code_stream;
            CodeGenerator &generator;

            void operator() (const Node::StmtExit *exit_statement) {
                std::string exit_label = generator.generate_label(TokenType::exit);
                code_stream << generator.generate_expression(exit_statement->expr);
                code_stream << generator.pop_stack("rdi");
                code_stream << "\tjmp _exit\n";
            }

            void operator() (const Node::StmtMut *mut_statement) {
                std::string variable_identifier = mut_statement->identifier.value.value();
                if (mut_statement->expr.has_value()) {
                    code_stream << generator.generate_expression(mut_statement->expr.value());
                }
                else {
                    code_stream << generator.move_stack(1);
                }

                generator.m_variables.add_variable(variable_identifier, generator.m_stack_pointer, generator.m_current_scope);
            }

            void operator() (const Node::StmtIdent *identifier_statement) {
                std::string variable_identifier = identifier_statement->identifier.value.value();
                auto iterator = generator.m_variables.get_variable(variable_identifier, generator.m_current_scope);
                const Variable& variable = iterator->second;
                std::stringstream offset;
                offset << "QWORD [rsp + " << (generator.m_stack_pointer - variable.stack_location) * 8 << "]";
                code_stream << generator.generate_expression(identifier_statement->expr);
                code_stream << generator.pop_stack(offset.str());
            }

            void operator() (const Node::Scope *scope_statement) {
                generator.begin_scope();
                for (const Node::Statement *stmt : scope_statement->stmts) {
                    code_stream << generator.generate_statement(stmt);
                }
                code_stream << generator.end_scope();
            }

            void operator() (const Node::StmtIf *if_statement) {
                std::stringstream buffer;
                code_stream << generator.generate_expression(if_statement->expr);
                std::string end_if_label = generator.generate_label(TokenType::if_);
                generator.m_labels.push_back(end_if_label);
                code_stream << generator.pop_stack("rax");
                code_stream << "\ttest rax, rax\n";
                generator.begin_scope();
                for (const Node::Statement *stmt : if_statement->scope->stmts) {
                    buffer << generator.generate_statement(stmt);
                }
                buffer << generator.end_scope();
                buffer << "jmp " << generator.m_labels.front() << "\n";
                if (if_statement->next.has_value()) {
                    buffer << generator.generate_if_next(if_statement->next.value());
                    code_stream << "\tjz " << generator.m_labels.back() << "\n";
                    generator.m_labels.pop_back();
                }
                else {
                    code_stream << "\tjz " << generator.m_labels.front() << "\n";
                }
                generator.m_labels.clear();
                code_stream << buffer.str();
                code_stream << "\n" << end_if_label << ":\n";
            }
        };
        std::stringstream code;
        StatementVisitor visitor{.code_stream = code, .generator = *this};
        std::visit(visitor, statement->var);

        return code.str();
    }

    [[nodiscard]] std::string generate_program() {
        m_asm_code << "global _start\n_start:\n";

        for (const Node::Statement *statement : m_program_node.statements) {
            m_asm_code << generate_statement(statement);
        }

        m_asm_code << "\n\tmov rdi, 0\n";
        m_asm_code << "\n_exit:\n";
        m_asm_code << "\tmov rax, 60\n";
        m_asm_code << "\tsyscall\n";

        return m_asm_code.str();
    }


private:
    int m_current_scope;
    Variables m_variables;
    size_t m_stack_pointer;
    std::stringstream m_asm_code;
    std::vector<std::string> m_labels;
    const Node::Program m_program_node;
    std::map<TokenType, int> m_label_map;

    std::string push_stack(const std::string &x64_register) {
        std::stringstream code;
        code << "\tpush " << x64_register << "\n";
        m_stack_pointer++; // One size is 64bit

        return code.str();
    }

    std::string pop_stack(const std::string &x64_register) {
        std::stringstream code;
        code << "\tpop " << x64_register << "\n";
        m_stack_pointer--; // One size is 64bit

        return code.str();
    }

    std::string move_stack(int offset) {
        std::stringstream code;
        code << "\tsub rsp, " << offset * 8 << "\n";
        m_stack_pointer = m_stack_pointer + offset; // One size is 64bit

        return code.str();
    }

    void begin_scope() {
        m_variables.scope_push({});
        m_current_scope++;
    }

    std::string end_scope() {
        std::stringstream code;
        auto scope_top = m_variables.get_top();
        size_t pop_count = scope_top.size();
        for(const std::string& identifier : scope_top){
            m_variables.delete_variable(identifier);
        }

        code << "\tadd rsp, " << pop_count * 8 << "\n";
        m_variables.scope_pop();
        m_stack_pointer -= pop_count;
        m_current_scope--;

        return code.str();
    }

    std::string generate_label(TokenType type) {
        std::string label;
        switch(type) {
            case TokenType::if_:
                label = "_end_if_label_" + std::to_string(m_label_map[type]);
                break;
            case TokenType::else_:
                label = "_else_label_" + std::to_string(m_label_map[type]);
                break;
            case TokenType::elif:
                label = "_elif_label_" + std::to_string(m_label_map[type]);
                break;
            case TokenType::for_:
                label = "_for_label_" + std::to_string(m_label_map[type]);
                break;
            case TokenType::while_:
                label = "_while_label_" + std::to_string(m_label_map[type]);
                break;
            case TokenType::exit:
                label = "_exit_" + std::to_string(m_label_map[type]);
                break;
            default:
                std::cerr << "cer: error: invalid label" << std::endl;
                exit(EXIT_FAILURE);
        }
        m_label_map[type] += 1;

        return label;
    }
};
