> [!CAUTION]
> The latest commit for some reason has seg fault issue while trying to compile. Idk i prolly fucked up some file upload and one or more files are with old changes that I am currently unable to fix it due to my the SSD that had my Arch dev setup crashed on me.
>
> So untill I get a new SSD I am sadly unable to fix the issue ;(

# Cerium
Cerium my own hobby programming language that i am building to learn and unravel how compilers actually work under the hood. th compiler is built on C++ and is called *`cer`* for now.

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

Executable will be `cer` in the `build/` directory.

# TODO

#### Urgent

- [ ] Imports
- [ ] Main function
- [ ] Macro definitions
- [ ] Conditional logic
  
#### Others
- [ ] More data types
- [ ] Funtions and funtion calls
- [ ] Better error messages from the compiler.


</br>

> [!NOTE]
>
><p>This for now is a hobby project which migh change in the future >to an actual language but lets not get ahead of ourselves.</p>