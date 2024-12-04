#include "buffer.h"

// Function to initialize the HTML buffer with the given initial size
int allocateHtmlBuffer(HtmlBuffer *htmlBuffer, size_t initialSize)
{
    htmlBuffer->Address = (u8 *)malloc(initialSize);
    if (!htmlBuffer->Address)
    {
        printf("Memory allocation failed.\n");
        return -1;
    }
    htmlBuffer->CurrentPosition = 0;
    htmlBuffer->BufferSize = initialSize;
    memset(htmlBuffer->Address, 0, initialSize); // Clear the buffer
    return 0;
}

// Function to reallocate the HTML buffer if it runs out of space
int reAllocateHtmlBuffer(HtmlBuffer *htmlBufferPtr)
{
    size_t newBufferSize = htmlBufferPtr->BufferSize * BUFFER_RESIZE_RATIO;
    printf("Buffer too small reallocating memory.\n");

    if (newBufferSize > MAX_BUFFER_SIZE)
    {
        printf("Error: Buffer cannot grow beyond %zu bytes. Current size: %zu bytes.\n", (size_t)MAX_BUFFER_SIZE, (size_t)htmlBufferPtr->BufferSize);
        return -1;
    }

    u8 *newAddress = (u8 *)realloc(htmlBufferPtr->Address, newBufferSize);
    if (!newAddress)
    {
        printf("Memory reallocation failed.\n");
        return -1;
    }

    htmlBufferPtr->Address = newAddress;
    htmlBufferPtr->BufferSize = newBufferSize;
    return 0;
}

// Function to append content to the HTML buffer
int appendLineToHtmlBuffer(HtmlBuffer *htmlBufferPtr, const char *contentLine, size_t contentLength)
{
    while (htmlBufferPtr->CurrentPosition + contentLength > htmlBufferPtr->BufferSize)
    {
        reAllocateHtmlBuffer(htmlBufferPtr);
    }

    memcpy(htmlBufferPtr->Address + htmlBufferPtr->CurrentPosition, contentLine, contentLength);
    htmlBufferPtr->CurrentPosition += contentLength;
    return 0;
}

// Function to strip newline characters from the end of the line
void stripNewLine(char *line)
{
    size_t lineLength = strlen(line);
    while (lineLength > 0 && (line[lineLength - 1] == '\n' || line[lineLength - 1] == '\r'))
    {
        line[--lineLength] = '\0';
    }
}

// Function to append an HTML tag to the HTML line
int appendToHtmlLine(char *htmlLine, u32 position, char *tag, size_t bufferSize)
{
    size_t tagLength = strlen(tag);
    if (position + tagLength >= bufferSize)
    {
        // Prevent writing over buffer size
        return position;
    }

    memcpy(htmlLine + position, tag, tagLength);
    return position += tagLength;
}
