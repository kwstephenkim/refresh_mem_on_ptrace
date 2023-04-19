## Compile Debuggee

clang -g3 -O0 ./debuggee/debuggee.c -o ./debuggee.x

## Compile Debugger

clang++ library.cc debugger.cc -o debugger.x


