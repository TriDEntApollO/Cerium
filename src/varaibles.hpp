#pragma once

#include <map>
#include <vector>

struct Variable {
    size_t stack_location;
};

class Variables {
    public:
        inline Variables() {

        }

        inline bool exists(std::string identifier){
            return m_variables.contains(identifier);
        }

        inline std::optional<const Variable>  is_declared(std::string identifier) {
            auto iterator = m_variables_map.find(identifier);
            if (iterator != m_variables_map.end()) {
                const Variable& variable = iterator->second;
                return variable;
            }
            return {};
        }

        inline void add_variable(std::string identifier, int stack_location) {
            Variable variable = { .stack_location = generator->m_stack_pointer };
            m_variables_map.insert({ variable_identifier });
        }

    private:
        std::map<std::string, std::optional<Variable>> m_variables_map;
};