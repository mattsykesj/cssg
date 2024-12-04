@echo off

:: Create the build directory if it doesn't exist
if not exist bin (
    mkdir bin
)

:: Create the output directory for release if necessary
if not exist bin\release (
    mkdir bin\release
)

:: Compile the source code
clang -I./include -O3 -DNDEBUG -o bin/release/cssg.exe src/main.c src/buffer.c src/parser.c

echo Build completed successfully.