#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <errno.h>
#include <stdint.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t
#define f32 float
#define d64 double
#define bool32 int32_t

#define HTML_ITALIC_TAG_OPEN "<em>"
#define HTML_ITALIC_TAG_CLOSE "</em>"
#define HTML_BOLD_TAG_OPEN "<strong>"
#define HTML_BOLD_TAG_CLOSE "</strong>"
#define HTML_PARAGRAPH_TAG_OPEN "<p>"
#define HTML_PARAGRAPH_TAG_CLOSE "</p>"

#define NEWLINE_TAG "\n"

#define BUFFER_RESIZE_RATIO 1.2

typedef struct
{
    u8 *Address;
    size_t CurrentPosition;
    size_t BufferSize;
} HtmlBuffer;

typedef enum
{
    LineType_Paragraph,
    LineType_Header,
    LineType_Blockquote,
    LineType_Code,
    LineType_HorizontalRule,
    LineType_Link,
    LineType_Image,
    LineType_OrderedList,
    LineType_UnorderedList,

} LineType;

static void stripNewLine(char *line)
{
    // Strip the newline character at the end of the line
    size_t lineLength = strlen(line);
    if (lineLength > 0 && line[lineLength - 1] == '\n')
    {
        line[lineLength - 1] = '\0'; // Remove the newline
    }
}

static int appendToHtmlLine(char *htmlLine, u32 position, char *tag, size_t bufferSize)
{
    if (position >= bufferSize)
    {
        // Prevent writing over buffer size
        return position;
    }

    snprintf(htmlLine + position, bufferSize, tag);
    return position += strlen(tag);
}

// static int reAllocateBuffer(u8 *buffer, size_t *bufferSize)
// {
//     printf("Buffer too small, reallocating new buffer %s", NEWLINE_TAG);
//     size_t newBufferSize = *bufferSize * BUFFER_RESIZE_RATIO;
//     u8 *newBuffer = (u8 *)realloc(buffer, newBufferSize);
//     if (!newBuffer)
//     {
//         printf("Memory reallocation failed. %s", NEWLINE_TAG);
//         return -1;
//     }

//     buffer = newBuffer;
//     *bufferSize = newBufferSize;
//     return 0;
// }

static int appendLineToBuffer(u8 *buffer, size_t *currentPosition, size_t *bufferSize, const char *contentLine, size_t contentLength)
{
    // if (*currentPosition + contentLength > *bufferSize)
    // {
    //     printf("Buffer too small, reallocating new buffer %s", NEWLINE_TAG);
    //     size_t newBufferSize = *bufferSize * 2;
    //     u8 *newBuffer = (u8 *)realloc(buffer, newBufferSize);
    //     if (!newBuffer)
    //     {
    //         printf("Memory reallocation failed. %s", NEWLINE_TAG);
    //         return -1;
    //     }

    //     buffer = newBuffer;
    //     *bufferSize = newBufferSize;
    // }

    memcpy(buffer + *currentPosition, contentLine, contentLength);
    *currentPosition += contentLength;
}

static int parseHeader(char *line, u8 *buffer, size_t *bufferSize, size_t *currentPosition)
{

    stripNewLine(line);
    // TODO(matt): add handling for not a valid header ###test no space.
    u32 headerCounter = 0;

    while (line[headerCounter] == '#')
    {
        headerCounter++;
    }

    if (headerCounter > 0 && headerCounter <= 6)
    {

        // Skip '#' characters and the following space
        char *textStartPtr = line + headerCounter + 1;
        char htmlLine[1024] = {0};
        snprintf(htmlLine, sizeof(htmlLine), "<h%d>%s</h%d>\n", headerCounter, textStartPtr, headerCounter);

        size_t htmlLineLength = strlen(htmlLine);
        appendLineToBuffer(buffer, currentPosition, bufferSize, htmlLine, htmlLineLength);
    }

    return 0;
}

static int parseParagraph(char *line, u8 *buffer, size_t *bufferSize, size_t *currentPosition)
{
    u8 i = 0;
    u32 htmlLinePosition = 0;
    stripNewLine(line);

    char htmlLine[1024] = {0};
    bool32 inItalic = 0;
    bool32 inBold = 0;

    htmlLinePosition = appendToHtmlLine(htmlLine, htmlLinePosition, HTML_PARAGRAPH_TAG_OPEN, sizeof(htmlLine));

    while (line[i] != '\0')
    {

        if (line[i] == '*')
        {
            if (line[i + 1] == '*')
            {
                // bold
                if (inBold == 1)
                {
                    inBold = 0;
                    htmlLinePosition = appendToHtmlLine(htmlLine, htmlLinePosition, HTML_BOLD_TAG_CLOSE, sizeof(htmlLine));
                    i += 2; // skip the second *
                    continue;
                }

                if (inBold == 0)
                {
                    inBold = 1;
                    htmlLinePosition = appendToHtmlLine(htmlLine, htmlLinePosition, HTML_BOLD_TAG_OPEN, sizeof(htmlLine));
                    i += 2; // skip the second *
                    continue;
                }
            }
            else
            {
                // italic
                if (inItalic == 1)
                {
                    inItalic = 0;
                    htmlLinePosition = appendToHtmlLine(htmlLine, htmlLinePosition, HTML_ITALIC_TAG_CLOSE, sizeof(htmlLine));
                    i++;
                    continue;
                }

                if (inItalic == 0)
                {
                    inItalic = 1;
                    htmlLinePosition = appendToHtmlLine(htmlLine, htmlLinePosition, HTML_ITALIC_TAG_OPEN, sizeof(htmlLine));
                    i++;
                    continue;
                }
            }
        }

        // process normal text
        snprintf(htmlLine + htmlLinePosition, sizeof(htmlLine), "%c", line[i]);
        i++;
        htmlLinePosition++;
    }

    htmlLinePosition = appendToHtmlLine(htmlLine, htmlLinePosition, HTML_PARAGRAPH_TAG_CLOSE, sizeof(htmlLine));
    htmlLinePosition = appendToHtmlLine(htmlLine, htmlLinePosition, NEWLINE_TAG, sizeof(htmlLine));

    // write HTML line to buffer
    size_t htmlLineLength = strlen(htmlLine);
    appendLineToBuffer(buffer, currentPosition, bufferSize, htmlLine, htmlLineLength);

    return 0;
}

int generateHtmlFromMarkdown(const char *markdownPath, const char *outputPath)
{
    // open md file
    FILE *file = fopen(markdownPath, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", strerror(errno));
        return 1;
    }

    // create html file
    FILE *htmlFile = fopen(outputPath, "w");
    if (htmlFile == NULL)
    {
        printf("Error opening file: %s\n", strerror(errno));
        fclose(file);
        return 1;
    }

    // allocate memory for reading md file into buffer
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    size_t bufferSize = fileSize * 2;
    fseek(file, 0, SEEK_SET);

    u8 *buffer = (u8 *)malloc(bufferSize);
    if (buffer == NULL)
    {
        printf("Memory allocation failed: %s\n", strerror(errno));
        fclose(file);
        fclose(htmlFile);
        return 1;
    }
    memset(buffer, 0, bufferSize); // Clear the buffer

    HtmlBuffer htmlBuffer = {0};

    htmlBuffer.Address = buffer;
    htmlBuffer.BufferSize = bufferSize;
    htmlBuffer.CurrentPosition = 0;

    // read the lines into the buffer
    size_t currentPosition = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char first = line[0];
        LineType lineType = LineType_Paragraph;

        if (first == '\0' || first == '\n')
        {
            continue; // Skip empty
        }

        if (first == '#')
            lineType = LineType_Header;
        else if (first == '`')
            lineType = LineType_Code;
        else if (first == '-')
            lineType = LineType_HorizontalRule;
        else if (first == '[')
            lineType = LineType_Link;
        else if (first == '!')
            lineType = LineType_Image;
        else if (first == '>')
            lineType = LineType_Blockquote;

        switch (lineType)
        {
        case LineType_Header:
            parseHeader(line, buffer, &bufferSize, &currentPosition);
            break;
        case LineType_Paragraph:
            parseParagraph(line, buffer, &bufferSize, &currentPosition);
            break;
        }
    }

    // buffer[currentPosition] = '\0'; // Null terminate the buffer

    // printf("File contents:\n%s\n", buffer); DEBUG
    fprintf(htmlFile, "%s", buffer);
    fclose(htmlFile);

    fclose(file);
    free(buffer);

    return 0;
}

int processMarkdownFile(WIN32_FIND_DATA *findDataPtr, char *newFileNamePtr, char *inputPathPtr, size_t inputPathSize, char *outputPathPtr, size_t outputPathSize, const char *basePath)
{
    printf("File found: %s\n", findDataPtr->cFileName);

    // remove .md replace with html
    snprintf(newFileNamePtr, strlen(findDataPtr->cFileName) + 6, "%.*s.html", strlen(findDataPtr->cFileName) - 3, findDataPtr->cFileName);

    snprintf(inputPathPtr, inputPathSize, "%s\\content\\%s", basePath, findDataPtr->cFileName);
    snprintf(outputPathPtr, outputPathSize, "%s\\output\\%s", basePath, newFileNamePtr);

    printf("input Path: %s\n", inputPathPtr);
    printf("output Path: %s\n", outputPathPtr);
    printf("converting: %s to %s\n", findDataPtr->cFileName, newFileNamePtr);
    generateHtmlFromMarkdown(inputPathPtr, outputPathPtr);

    return 0;
}

int processMarkdownDirectory(const char *basePath)
{
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\content\\*.md", basePath); // Only look for .md files
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("No files found.\n");
        return 1;
    }
    else
    {
        char inputPath[MAX_PATH];
        char outputPath[MAX_PATH];
        char newFileName[MAX_PATH];

        // process first file
        processMarkdownFile(&findData, newFileName, inputPath, sizeof(inputPath), outputPath, sizeof(outputPath), basePath);
        while (FindNextFile(hFind, &findData) != 0)
        { // Process remaining files

            processMarkdownFile(&findData, newFileName, inputPath, sizeof(inputPath), outputPath, sizeof(outputPath), basePath);
        }
    }

    // Close the search handle
    FindClose(hFind);
    return 0;
}

int generatePage(const char *basePath)
{
    char outputPath[MAX_PATH] = {0};
    char layoutPath[MAX_PATH] = {0};

    snprintf(outputPath, sizeof(outputPath), "%s\\public\\%s", basePath, "index.html");
    snprintf(layoutPath, sizeof(layoutPath), "%s\\partials\\%s", basePath, "_layout.html");

    // open layout file
    FILE *layoutFile = fopen(layoutPath, "r");
    if (layoutFile == NULL)
    {
        printf("Error reading layout file: %s\n", strerror(errno));
        return 1;
    }

    // allocate memory for reading layout file into buffer
    fseek(layoutFile, 0, SEEK_END);
    long fileSize = ftell(layoutFile);
    size_t bufferSize = fileSize * 4;
    fseek(layoutFile, 0, SEEK_SET);

    u8 *buffer = (u8 *)malloc(bufferSize);
    if (buffer == NULL)
    {
        printf("Memory allocation failed: %s\n", strerror(errno));
        fclose(layoutFile);
        return 1;
    }
    memset(buffer, 0, bufferSize); // Clear the buffer

    // read the lines into the buffer
    // need to check for cases where we have text and {{}} on the same line outside of the braces.
    size_t currentPosition = 0;
    char line[256];
    while (fgets(line, sizeof(line), layoutFile) != NULL)
    {
        u32 i = 0;
        bool32 inPartial = 0;
        bool32 partialWasInserted = 0;
        char partialName[256] = {0};
        char partialDir[MAX_PATH] = {0};
        u32 contentLength = strlen(line);
        u32 nameIndex = 0;

        while (line[i] != '\n')
        {
            if (line[i] == '{' && inPartial == 0)
            {
                if (line[i + 1] == '{')
                {
                    // check for partial
                    inPartial = 1;
                    i += 4; // skip to name
                    // continue;
                }
            }

            if (inPartial)
            {
                partialName[nameIndex] = line[i];
                nameIndex++;

                if (line[i + 1] == '\"')
                {
                    inPartial = 0;

                    // create html file, reading in binary to avoid any transforms
                    snprintf(partialDir, sizeof(partialDir), "%s\\output\\%s", basePath, partialName);
                    FILE *partialFile = fopen(partialDir, "rb");
                    if (partialFile == NULL)
                    {
                        printf("Template file %s not found in output folder: %s\n", partialName, strerror(errno));
                        return 1;
                    }

                    // get size of file to read into buffer
                    fseek(partialFile, 0, SEEK_END);
                    long partialFileSize = ftell(partialFile);
                    size_t partialSize = partialFileSize;
                    fseek(partialFile, 0, SEEK_SET);

                    // TODO(matt) handle if buffer is too small
                    if (currentPosition + partialSize > bufferSize)
                    {
                        // reAllocateBuffer(buffer, &bufferSize);
                    }
                    fread(buffer + currentPosition, sizeof(char), partialSize, partialFile);
                    currentPosition += partialSize;

                    partialWasInserted = 1;

                    fclose(partialFile);
                }
            }

            i++;
        }

        // skip appending lines where we inserted partials
        if (partialWasInserted == 0)
        {
            char htmlLine[1024] = {0};
            snprintf(htmlLine, sizeof(htmlLine), "%s", line);

            size_t htmlLineLength = strlen(htmlLine);
            appendLineToBuffer(buffer, &currentPosition, &bufferSize, htmlLine, htmlLineLength);
        }
    }

    buffer[currentPosition] = '\0'; // Null terminate the buffer

    printf("File contents:\n%s\n", buffer);

    FILE *indexFile = fopen(outputPath, "wb");
    if (indexFile == NULL)
    {
        printf("Error opening file for writing: %s\n", outputPath);
        return 1;
    }

    // Write the buffer to the file
    size_t bytesWritten = fwrite(buffer, 1, bufferSize, indexFile);
    if (bytesWritten != bufferSize)
    {
        printf("Error writing to file: Only %zu out of %zu bytes were written\n", bytesWritten, bufferSize);
    }

    // Close the files
    fclose(indexFile);
    fclose(layoutFile);
    free(buffer);

    return 0;
}