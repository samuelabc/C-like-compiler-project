# C-like-compiler-project
A compiler which compiled a basic C-like programming language into MIPS assembly language. The C-like language to be compiled is placed in src/testfile.txt, while the final output(MIPS instructions) is placed in src/mips.txt. Lexical and syntax error is logged in src/error.txt. 
Token extracted and syntax analyzed during lexical analysis and syntax analysis phase is placed in src/output.txt. Intermediate code generated after semantic analysis phase is placed in src/intermediate.txt, the intermediate code is then translated into MIPS code by the target code generator.
