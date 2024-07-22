#pragma once

#include <string>
#include <sstream>
#include <unordered_map>

#include "parser.hpp"


class CodeGenerator {
public:
    inline explicit CodeGenerator(Node::Program program_node) : m_program_node(std::move(program_node)) {
        m_stack_pointer = 0;
        m_variables_map = {};
    }

    [[nodiscard]] std::string generate_term(const Node::Term *term) {
        struct ExpressionVisitor {
            std::stringstream &code_stream;
            CodeGenerator *generator;

            void operator() (const Node::TermInt *int_term) const {
                code_stream << "\tmov rax, " << int_term->int_lit.value.value() << "\n";
                code_stream << generator->push_stack("rax");
            }

            void operator() (const Node::TermIdent *identifier_term) {
                std::string variable_identifier = identifier_term->identifier.value.value();
                auto iterator = generator->m_variables_map.find(variable_identifier);
                if (iterator == generator->m_variables_map.end()) {
                    std::cerr << "cer: error: undeclared identifier '" << variable_identifier << "'" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                const Variable& variable = iterator->second;
                std::stringstream offset;
                offset << "QWORD [rsp + " << (generator->m_stack_pointer - variable.stack_location) * 8 << "]";
                code_stream << generator->push_stack(offset.str());
            }
        };
        std::stringstream code;
        ExpressionVisitor visitor{.code_stream = code, .generator = this};
        std::visit(visitor, term->var);

        return code.str();
    }

    [[nodiscard]] std::string generate_binary_expression(const Node::BinExpr *binary_expression) {
        struct BinExpressionVisitor {
            std::stringstream &code_stream;
            CodeGenerator *generator;

            void operator() (const Node::BinAdd *binary_add) const {
                code_stream << generator->generate_expression(binary_add->left_side);
                code_stream << generator->generate_expression(binary_add->right_side);
                code_stream << generator->pop_stack("rbx");
                code_stream << generator->pop_stack("rax");
                code_stream << "\tadd rax, rbx\n";
                code_stream << generator->push_stack("rax");
            }

            void operator() (const Node::BinSubtract *binary_subtract) const {
                std::cerr << "cer: operator no implemented yet lol" << std::endl;
                exit(EXIT_FAILURE);
            }

            void operator() (const Node::BinMultiply *binary_multiply) const {
                std::cerr << "cer: operator no implemented yet lol" << std::endl;
                exit(EXIT_FAILURE);
            }

            void operator() (const Node::BinDivide *binary_divide) const {
                std::cerr << "cer: operator no implemented yet lol" << std::endl;
                exit(EXIT_FAILURE);
            }
        };
        std::stringstream code;
        BinExpressionVisitor visitor{.code_stream = code, .generator = this};
        std::visit(visitor, binary_expression->bin_expr);

        return code.str();
    }

    [[nodiscard]] std::string generate_expression(const Node::Expression *expression) {
        struct ExpressionVisitor {
            std::stringstream &code_stream;
            CodeGenerator *generator;

            void operator() (const Node::Term *term) const {
                code_stream << generator->generate_term(term);
            }

            void operator() (const Node::BinExpr *binary_expression) {\
                code_stream << generator->generate_binary_expression(binary_expression);
            }
        };
        std::stringstream code;
        ExpressionVisitor visitor{.code_stream = code, .generator = this};
        std::visit(visitor, expression->var);

        return code.str();
    }

    [[nodiscard]] std::string generate_statement(const Node::Statement *statement) {
        struct StatementVisitor {
            std::stringstream &code_stream;
            CodeGenerator *generator;

            void operator() (const Node::StmtExit *exit_statement) {
                code_stream << generator->generate_expression(exit_statement->expr);
                code_stream << "\n_exit_program:\n";
                code_stream << "\tmov rax, 60\n";
                code_stream << generator->pop_stack("rdi");
                code_stream << "\tsyscall\n";
            }

            void operator() (const Node::StmtConst *const_statement) {
                std::string variable_identifier = const_statement->identifier.value.value();
                if (generator->m_variables_map.contains(variable_identifier)) {
                    std::cerr << "cer: error: multiple definitions of identifier '" << variable_identifier << "'" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                code_stream << generator->generate_expression(const_statement->expr);

                Variable variable = { .stack_location = generator->m_stack_pointer };
                generator->m_variables_map.insert({ variable_identifier, variable });

            }
        };
        std::stringstream code;
        StatementVisitor visitor{.code_stream = code, .generator = this};
        std::visit(visitor, statement->var);

        return code.str();
    }

    [[nodiscard]] std::string generate_program() {
        m_asm_code << "global _start\n_start:\n";

        for (const Node::Statement *statement : m_program_node.statements) {
            m_asm_code << generate_statement(statement);
        }

        m_asm_code << "\n_exit_default:\n";
        m_asm_code << "\tmov rax, 60\n";
        m_asm_code << "\tmov rdi, 0\n";
        m_asm_code << "\tsyscall\n";

        return m_asm_code.str();
    }


private:
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

    struct Variable {
        size_t stack_location;
    };

    const Node::Program m_program_node;
    std::stringstream m_asm_code;
    size_t m_stack_pointer;
    std::unordered_map<std::string, Variable> m_variables_map;
};
