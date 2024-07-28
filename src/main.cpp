#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>

#include "./token.hpp"
#include "./error.hpp"
#include "./tokenize.hpp"
#include "./parser.hpp"
#include "./codegen.hpp"
#include "./varaibles.hpp"


int IsValidFile(std::string filename) {
    std::filesystem::path filepath = std::move(filename);
    std::string extension = filepath.extension();
    filename = std::move(filepath);
    if (extension == ".crm"){
        return 1;
    }
    return 0;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "cer: error: no input files" << std::endl;
        return 1;
    }

    int arg = 1;
    int debug_flag = 0;
    std::string source_file;
    std::string output_file;
    std::string linker_command;

    while (arg < argc) {
        if (strcmp(argv[arg], "-d") == 0) {
            debug_flag = 1;
        }
        else if (strcmp(argv[arg], "-o") == 0) {
            output_file = argv[arg + 1];
            source_file = argv[arg + 2];
            arg = arg + 2;
        }
        else {
            source_file = argv[arg];
            output_file = "out";
        }
        arg++;
    }

    if (!IsValidFile(source_file)){
        std::cerr << "cer: error: invalid file type" << std::endl;
        return 2;
    }

    std::stringstream contents_stream;
    std::fstream input(source_file, std::ios::in);
    if (!input.is_open()) {
        std::cout << "cer: error: failed to open the file" << std::endl;
        return 3;
    }
    contents_stream << input.rdbuf();
    input.close();
    std::string contents = contents_stream.str();

    Tokenizer tokenizer(std::move(contents), source_file);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens), source_file);
    std::pair<Node::Program, Variables> ast_vars_pair = parser.parse_program();

    Node::Program ast = ast_vars_pair.first;
    Variables variables = ast_vars_pair.second;

    if (!error_flag) {
        CodeGenerator generator(ast, variables);

        std::fstream file("out.asm", std::ios::out);
        file << generator.generate_program();
        file.close();


        linker_command = "ld -o " + output_file + " out.o";
        system("nasm -felf64 out.asm");
        system(linker_command.c_str());
        if (!debug_flag) {
            system("rm out.o");
            system("rm out.asm");
        }
    }

    return 0;
}

