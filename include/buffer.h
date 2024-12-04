#ifndef HTML_BUFFER_H
#define HTML_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_RESIZE_RATIO 1.4
#define MAX_BUFFER_SIZE (200 * 1024 * 1024) // 200 MB

typedef unsigned char u8;
typedef unsigned int u32;

// Structure for the HTML buffer
typedef struct
{
    u8 *Address;            // Pointer to the buffer
    size_t CurrentPosition; // Current position in the buffer
    size_t BufferSize;      // Current size of the buffer
} HtmlBuffer;

// Function declarations

// Initializes the HTML buffer with the given initial size
int allocateHtmlBuffer(HtmlBuffer *htmlBuffer, size_t initialSize);

// Reallocates the HTML buffer if it runs out of space
int reAllocateHtmlBuffer(HtmlBuffer *htmlBufferPtr);

// Appends content to the HTML buffer
int appendLineToHtmlBuffer(HtmlBuffer *htmlBufferPtr, const char *contentLine, size_t contentLength);

// Strips newline characters from the end of the line
void stripNewLine(char *line);

// Appends an HTML tag to the HTML line
int appendToHtmlLine(char *htmlLine, u32 position, char *tag, size_t bufferSize);

#endif
