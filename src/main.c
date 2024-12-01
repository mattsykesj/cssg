#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

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

static int parseHeader(char *line, u8 *buffer, size_t *currentPosition)
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

        size_t htmlLineContentLength = strlen(htmlLine);

        memcpy(buffer + *currentPosition, htmlLine, htmlLineContentLength);
        *currentPosition += htmlLineContentLength;
    }

    return 0;
}

static int parseParagraph(char *line, u8 *buffer, size_t *currentPosition)
{
    u8 i = 0;
    stripNewLine(line);

    char htmlLine[1024] = {0};

    while (line[i] != '\0')
    {
        snprintf(htmlLine, sizeof(htmlLine), "%c", line[i]);
        i++;
    }

    // snprintf(htmlLine, sizeof(htmlLine), "%s\n", line);

    size_t htmlLineContentLength = strlen(htmlLine);

    memcpy(buffer + *currentPosition, htmlLine, htmlLineContentLength);
    *currentPosition += htmlLineContentLength;

    // // Skip '#' characters and the following space
    // char *textStartPtr = line + headerCounter + 1;
    // char htmlLine[1024] = {0};
    // snprintf(htmlLine, sizeof(htmlLine), "<h%d>%s</h%d>\n", headerCounter, textStartPtr, headerCounter);

    // size_t htmlLineContentLength = strlen(htmlLine);

    // memcpy(buffer + *currentPosition, htmlLine, htmlLineContentLength);
    // *currentPosition += htmlLineContentLength;

    return 0;
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("\n*argv = %s\n", *argv);

    // open md file
    FILE *file = fopen("content/test.md", "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", strerror(errno));
        return 1;
    }

    // allocate memory for reading md file into buffer
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8 *buffer = (u8 *)malloc(fileSize + 1);

    if (buffer == NULL)
    {
        printf("Memory allocation failed: %s\n", strerror(errno));
        fclose(file);
        return 1;
    }

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
            parseHeader(line, buffer, &currentPosition);
            break;
        case LineType_Paragraph:
            parseParagraph(line, buffer, &currentPosition);
            break;
        }

        // while(line[star_counter] == '*')
        // {
        //     star_counter++;
        // }

        // //italic
        // if(star_counter == 1)
        // {
        //     // Remove the **/n at the end of the string
        //     size_t lineLength = strlen(line);
        //     if (lineLength > 0 && line[lineLength - 1] == '\n' &&
        //                            line[lineLength - 2] == '*')
        //     {
        //         line[lineLength - 2] = '\0';
        //     }
        //     else
        //     {
        //         //parsing error not valid italic line.
        //     }

        //     u8 *textStartPtr = line + star_counter;
        //     u8 htmlLine[1024] = {0};
        //     snprintf(htmlLine, sizeof(htmlLine), "<i>%s</i>\n", textStartPtr);

        //     size_t htmlLineContentLength = strlen(htmlLine);

        //     memcpy(buffer + currentPosition, htmlLine, htmlLineContentLength);
        //     currentPosition += htmlLineContentLength;
        //     continue;
        // }

        // //bold
        // if(star_counter == 2)
        // {
        //     // Remove the **/n at the end of the string
        //     size_t lineLength = strlen(line);
        //     if (lineLength > 0 && line[lineLength - 1] == '\n' &&
        //                            line[lineLength - 2] == '*' &&
        //                            line[lineLength - 3] == '*')
        //     {
        //         line[lineLength - 3] = '\0';
        //     }
        //     else
        //     {
        //         //parsing error not valid bold line.
        //     }

        //     u8 *textStartPtr = line + star_counter;
        //     u8 htmlLine[1024] = {0};
        //     snprintf(htmlLine, sizeof(htmlLine), "<b>%s</b>\n", textStartPtr);

        //     size_t htmlLineContentLength = strlen(htmlLine);

        //     memcpy(buffer + currentPosition, htmlLine, htmlLineContentLength);
        //     currentPosition += htmlLineContentLength;
        //     continue;
        // }

        // headers
    }

    buffer[currentPosition] = '\0'; // Null terminate the buffer

    printf("File contents:\n%s\n", buffer);
    printf("the file size is %ld!\n", fileSize);

    fclose(file);
    free(buffer);

    return 0;
}
