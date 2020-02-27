Trying to make a fast and tiny interpreter, possibly for wasm.
Currently able to calculate fib seq. recursivly in about 10 seconds with msvc and 8.3 with clang.
Either use visual studio or the following
```
clang/gcc testvm.c test_fib.c -w -o testvm.exe -O4 && testvm.exe
```
