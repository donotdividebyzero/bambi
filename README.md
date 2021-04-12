#### To start please build first a brumbrum
```Bash
gcc build.c -o compiler
```
#This command will build compiler for brumbrum
```Bash
./compiler
```
---

### Possible commands in brumbrum
```Bash
Usage:
  Available subcommands:
    ./brumbrum help     - Usage, description
    ./brumbrum tests    - Command that build and run test suite
    ./brumbrum runtime  - Brumbrum runtime
```


### @TODO
0.5 Fix implicitly declaration while <string.h> is present in vm.c file.
```Bash
mkdir -p build/./src/
gcc -I./src -MD -MP  src/runtime.c -o build/./src/runtime.c.o
Undefined symbols for architecture arm64:
  "_main", referenced from:
     implicit entry/start for main executable
  "_token_stream_tokenize", referenced from:
      _runtime in runtime-9e81ee.o
ld: symbol(s) not found for architecture arm64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
make: *** [build/./src/runtime.c.o] Error 1
```
1. [ ] - Tokenizer
2. [ ] - AST
3. [ ] - Interpreter
4. Test runner need to do some asserts and printing some valuable information about test executed
