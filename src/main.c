#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "parser.h"

#ifdef DEBUG
char *contentDir = "../../content";
char *outputDir = "../../output";
#else
char *contentDir = "./content";
char *outputDir = "./output";
#endif

#define BASE_DIR "D:\\work\\cssg"

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("\n*argv = %s\n", *argv);

    processMarkdownDirectory(BASE_DIR);
    generatePage(BASE_DIR);

    printf("Press any key to continue...\n");
    getchar(); // Wait for any key press
    printf("You pressed a key. Program will exit now.\n");

    return 0;
}
