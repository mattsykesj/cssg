#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <errno.h>
#include <stdint.h>

#include "buffer.h"

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

#define BUFFER_RESIZE_RATIO 1.4
#define MAX_BUFFER_SIZE (200 * 1024 * 1024) // 200 MB

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

static int parseHeader(char *line, HtmlBuffer *htmlBufferPtr)
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

        appendLineToHtmlBuffer(htmlBufferPtr, htmlLine, htmlLineLength);
    }

    return 0;
}

static int parseParagraph(char *line, HtmlBuffer *htmlBufferPtr)
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
    appendLineToHtmlBuffer(htmlBufferPtr, htmlLine, htmlLineLength);

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
    size_t bufferSize = fileSize;
    fseek(file, 0, SEEK_SET);

    HtmlBuffer htmlBuffer = {0};
    allocateHtmlBuffer(&htmlBuffer, bufferSize);

    // read the lines into the buffer
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
            parseHeader(line, &htmlBuffer);
            break;
        case LineType_Paragraph:
            parseParagraph(line, &htmlBuffer);
            break;
        }
    }

    htmlBuffer.Address[htmlBuffer.CurrentPosition] = '\0'; // Null terminate the buffer

    // printf("File contents:\n%s\n", buffer); DEBUG
    fprintf(htmlFile, "%s", htmlBuffer.Address);
    fclose(htmlFile);

    fclose(file);
    free(htmlBuffer.Address);

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
    size_t bufferSize = fileSize;
    fseek(layoutFile, 0, SEEK_SET);

    HtmlBuffer htmlBuffer;
    allocateHtmlBuffer(&htmlBuffer, bufferSize);

    // read the lines into the buffer
    // need to check for cases where we have text and {{}} on the same line outside of the braces.
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

                    if (htmlBuffer.CurrentPosition + partialSize > htmlBuffer.BufferSize)
                    {
                        reAllocateHtmlBuffer(&htmlBuffer);
                    }

                    fread(htmlBuffer.Address + htmlBuffer.CurrentPosition, sizeof(char), partialSize, partialFile);
                    htmlBuffer.CurrentPosition += partialSize;

                    partialWasInserted = 1;

                    fclose(partialFile);
                }
            }

            i++;
        }

        // If we didnt process a partial add like normal, only skip when we process partial views
        if (partialWasInserted == 0)
        {
            char htmlLine[1024] = {0};
            snprintf(htmlLine, sizeof(htmlLine), "%s", line);

            size_t htmlLineLength = strlen(htmlLine);
            appendLineToHtmlBuffer(&htmlBuffer, htmlLine, htmlLineLength);
        }
    }

    htmlBuffer.Address[htmlBuffer.CurrentPosition] = '\0'; // Null terminate the buffer

    // printf("File contents:\n%s\n", htmlBuffer.Address); DEBUG

    FILE *indexFile = fopen(outputPath, "w");
    if (indexFile == NULL)
    {
        printf("Error opening file for writing: %s\n", outputPath);
        return 1;
    }

    // Write the buffer to the file
    size_t validLength = strlen((char *)htmlBuffer.Address);
    size_t bytesWritten = fwrite(htmlBuffer.Address, sizeof(char), validLength, indexFile);
    if (bytesWritten != validLength)
    {
        printf("Error writing to file: Only %zu out of %zu bytes were written\n", bytesWritten, validLength);
    }

    // Close the files
    fclose(indexFile);
    fclose(layoutFile);
    free(htmlBuffer.Address);

    return 0;
}
