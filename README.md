# Cerium
Cerium is a compiler built in C++ for my own programming language that i am building to learn and unravel how compilers actually work under the hood. 

P.S. this project was acutally inspired by [hydrogen](https://github.com/orosmatthew/hydrogen-cpp) by [orosmatthew](https://github.com/orosmatthew).

# Support

Currently only supports

- Operating systems: Linux
- Architectures: x86_64 

# Features
Current working features are:

- Varaible declaration
- Varaible initialization
- Working Scopes
- Arithmetic operations
- Bitwise operations
- Operator precendenc
- If else and else if
- Comments
- Some what readable error messages

## Build

Requires `nasm` and `ld` on a Linux operating system.

```bash & zsh
git clone https://github.com/TriDEntApollO/Cerium.git
cd Cerium
mkdir build
cmake -S . -B build
cmake --build build
```

Executable will be `hydro` in the `build/` directory.

</br>

> [!NOTE]
>
><p>This for now is a hobby project which migh change in the future >to an actual language but lets not get ahead of ourselves.</p>