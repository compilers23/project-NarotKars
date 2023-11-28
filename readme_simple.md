simple assignment is this:

you write a program which is a forth like language compiler.

your program will open a file supplied to it as a commandline argument.
let's say the program file is called test.txt.
then if you call your program like this:

```
./prg test.txt
```

it will open the file and read it.
it will convert the file to test.s file which contains assembly

then it sould call 'as' assembler to compile the source to test.o object file.
then it should call 'ld' linker to executable.
the file can contain

* numbers
* math operations: +, -, *
* some forth commands: swap, nip, tuck, dup, drop, over
* ".s" means: print what is at the top of the stack

you have a gforth crash course tutorial: https://gforth.org/manual/Crash-Course-Tutorial.html

you can try gforth on our server, it is installed there.

important. in this repository you have the example of how to print.
you can have this print.s as your runtime library.
it'll be linked to the code produced by your compiler.

and your compiler should generate the code which calls function print
like it is shown in main.s



