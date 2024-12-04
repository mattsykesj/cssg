@echo off

:: Create the build directory if it doesn't exist
if not exist bin (
    mkdir bin
)

:: Create the output directory for release if necessary
if not exist bin/debug (
    mkdir bin/debug
)

:: Compile the source code
clang -I./include -g -o bin/debug/cssg.exe src/main.c src/buffer.c src/parser.c

echo Build completed successfully.