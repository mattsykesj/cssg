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
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8 *buffer = (u8 *)malloc(file_size + 1);

    if (buffer == NULL)
    {
        printf("Memory allocation failed: %s\n", strerror(errno));
        fclose(file);
        return 1;
    }

    // read the lines into the buffer
    size_t current_position = 0;
    u8 line[256];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        u32 line_index = 0; 
        u32 header_counter = 0;
        // u32 star_counter = 0;

        for(line_index; line[line_index] != '\0'; line_index++)
        {
            if(line[line_index] == '#')
            {
                header_counter++;
            }
        }



        // while(line[star_counter] == '*')
        // {
        //     star_counter++;
        // }

        // //italic
        // if(star_counter == 1)
        // {
        //     // Remove the **/n at the end of the string
        //     size_t line_length = strlen(line);
        //     if (line_length > 0 && line[line_length - 1] == '\n' && 
        //                            line[line_length - 2] == '*')
        //     {
        //         line[line_length - 2] = '\0';
        //     }
        //     else
        //     {
        //         //parsing error not valid italic line.
        //     }

        //     u8 *text_start_ptr = line + star_counter;
        //     u8 html_line[1024] = {0};
        //     snprintf(html_line, sizeof(html_line), "<i>%s</i>\n", text_start_ptr);

        //     size_t html_line_content_length = strlen(html_line);

        //     memcpy(buffer + current_position, html_line, html_line_content_length);
        //     current_position += html_line_content_length;
        //     continue;
        // }

        // //bold
        // if(star_counter == 2)
        // {
        //     // Remove the **/n at the end of the string
        //     size_t line_length = strlen(line);
        //     if (line_length > 0 && line[line_length - 1] == '\n' && 
        //                            line[line_length - 2] == '*' && 
        //                            line[line_length - 3] == '*')
        //     {
        //         line[line_length - 3] = '\0';
        //     }
        //     else
        //     {
        //         //parsing error not valid bold line.
        //     }

        //     u8 *text_start_ptr = line + star_counter;
        //     u8 html_line[1024] = {0};
        //     snprintf(html_line, sizeof(html_line), "<b>%s</b>\n", text_start_ptr);

        //     size_t html_line_content_length = strlen(html_line);

        //     memcpy(buffer + current_position, html_line, html_line_content_length);
        //     current_position += html_line_content_length;
        //     continue;
        // }

        //headers
        if (header_counter > 0 && header_counter <= 6)
        {
            // Strip the newline character at the end of the line
            size_t line_length = strlen(line);
            if (line_length > 0 && line[line_length - 1] == '\n')
            {
                line[line_length - 1] = '\0'; // Remove the newline
            }

            u8 *text_start_ptr = line + header_counter + 1;
            u8 html_line[1024] = {0};
            snprintf(html_line, sizeof(html_line), "<h%d>%s</h%d>\n", header_counter, text_start_ptr, header_counter);

            size_t html_line_content_length = strlen(html_line);

            memcpy(buffer + current_position, html_line, html_line_content_length);
            current_position += html_line_content_length;
            continue;
        }

        size_t line_content_length = strlen(line);

        memcpy(buffer + current_position, line, line_content_length);
        current_position += line_content_length;
    }

    buffer[current_position] = '\0'; // Null terminate the buffer

    printf("File contents:\n%s\n", buffer);
    printf("the file size is %ld!\n", file_size);

    fclose(file);
    free(buffer);

    return 0;
}