#pragma once

#include <map>
#include <stack>
#include <vector>

struct Variable {
    size_t stack_location;
};

class Variables {
    public:
        inline Variables() = default;
        inline ~Variables() = default;

        inline bool exists(std::string identifier, int current_scope){
            for (int scope = current_scope; scope >= 0; scope--) {
                std::string current_identifier = generate_name(identifier, scope);
                auto iterator = m_variables_map.find(current_identifier);
                if (iterator != m_variables_map.end()) {
                    return true;
                }
            }

            return false;
        }

        inline bool is_valid(std::string identifier, int scope) {
            return !m_variables_map.contains(generate_name(identifier, scope));
        }

        inline void declare_variable(std::string identifier, int scope) {
            m_variables_map.insert({ generate_name(identifier, scope), Variable {} });
        }

        std::map<std::string, Variable>::iterator get_variable(const std::string& identifier, int current_scope) {
            for (int scope = current_scope; scope >= 0; scope--) {
                std::string current_identifier = generate_name(identifier, scope);
                auto iterator = m_variables_map.find(current_identifier);
                if (iterator != m_variables_map.end()) {
                    return iterator;
                }
            }

            return {};
        }

        inline void add_variable(std::string identifier, size_t stack_location, int scope) {
            std::string variable_name = generate_name(identifier, scope);
            m_variables_map[variable_name].stack_location = stack_location;
            if (scope != 0) {
                m_scopes.top().push_back(variable_name);
            }
        }

        void delete_variable(std::string identifier) {
            m_variables_map.erase(identifier);
        }

        std::vector<std::string> get_top() {
            return m_scopes.top();
        }

        void scope_push(std::vector<std::string> element) {
            m_scopes.push(element);
        }

        void scope_pop() {
            m_scopes.pop();
        }


    private:
        std::stack<std::vector<std::string>> m_scopes{};
        std::map<std::string, Variable> m_variables_map{};

        inline static std::string generate_name(std::string identifier, int scope) {
            return identifier + "_sc_" + std::to_string(scope);
        }
};