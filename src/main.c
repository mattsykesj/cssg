#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "parser.h"

#define MAX_PATH_LEN 260

int main(int argc, char *argv[])
{
    // Buffer to store the current working directory
    char baseDir[MAX_PATH_LEN];

    // Get the current working directory
    if (GetCurrentDirectory(MAX_PATH_LEN, baseDir) == 0)
    {
        fprintf(stderr, "Error: Unable to get current directory. Error code: %d\n", GetLastError());
        return 1;
    }

    // Print the current directory (for debugging purposes)
    printf("Current directory: %s\n", baseDir);

    processMarkdownDirectory(baseDir);
    generatePage(baseDir);

    printf("Successfully generated index.html\n");
    return 0;
}
