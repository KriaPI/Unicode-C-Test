#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>
#include <locale.h>
#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>


// TODO: FIX the conversion functions so that emojis don't end up the wrong emoji! VERY IMPORTANT!

// note: mblen is not thread-safe!

//enum {};

typedef struct
{
    char32_t* string_content;
    size_t    length_used;
    size_t    length_total;
    size_t    size_allocated;
    bool      initialized;
} 
SU32_mutable;

typedef struct
{
    const char* string_content;
    size_t      multibyte_chars_total;
    size_t      size_allocated;
    bool        initialized;
} 
SU8_immutable;

size_t SU8_multibyte_length(const char* stringMB, size_t bytes)
{
    // Note: 
    // 1 byte  0xxxxxxx 
    // 2 bytes 110xxxxx 
    // 3 bytes 1110xxxx 
    // 4 bytes 11110xxx 
    // Gets the length of a multibyte utf-8 encoded string.
    // Uses bitmasks.

    size_t length = 0;
    size_t i = 0;

    while (i < bytes)
    {
        length += 1;
        // Test if a character is 1, 2, 3 or 4 bytes
        if ((0b10000000 & stringMB[i]) == 0)
        {
            i += 1;
            continue;
        }
        if ((0b11100000 & stringMB[i]) == 0b11000000)
        {
            i += 2;
            continue;
        }
        if ((0b11110000 & stringMB[i]) == 0b11100000)
        {
            i += 3;
            continue;
        }
        if ((0b11111000 & stringMB[i]) == 0b11110000)
        {
            i += 4;
        }
    }

    return length;
}

SU8_immutable* stringU8_create(const char* stringMB)
{
    // Store a string literal within the struct SU8_immutable
    // and populate all struct members with a designated initialization.
    size_t bytes          = strlen(stringMB);
    SU8_immutable* object = malloc(sizeof(SU8_immutable)); 

    *object = (SU8_immutable) 
    {
        .string_content        = stringMB,
        .multibyte_chars_total = SU8_multibyte_length(stringMB, bytes),
        .size_allocated        = bytes,
        .initialized           = true
    };

    return object;
}

SU8_immutable* stringU8_create_conversion(const char* stringMB, size_t characters, size_t bytes)
{
    // Store a string literal within the struct SU8_immutable
    // and populate all struct members with a designated initialization.
    SU8_immutable* object = malloc(sizeof(SU8_immutable)); 

    *object = (SU8_immutable) 
    {
        .string_content        = stringMB,
        .multibyte_chars_total = characters,
        .size_allocated        = bytes,
        .initialized           = true
    };

    return object;
}

SU32_mutable* stringU32_create(char32_t* string, size_t characters)
{
    // Store a string literal within the struct SU8_immutable
    // and populate all struct members with a designated initialization.
    SU32_mutable* object = malloc(sizeof(SU32_mutable)); 

    *object = (SU32_mutable) 
    {
        .string_content = string,
        .length_used    = characters,
        .size_allocated = characters * sizeof(char32_t),
        .initialized    = true
    };

    return object;
}


SU32_mutable* SU8_ConvertEncoding(SU8_immutable* stringToConvert)
{
    size_t utf8_chars        = stringToConvert->multibyte_chars_total;
    size_t utf32_string_size = utf8_chars * sizeof(char32_t);
    size_t utf8_index        = 0;
    size_t utf8_index_temp   = 0;

    char32_t* utf32_string = malloc(utf32_string_size);
    char32_t* string_end   = &utf32_string[utf8_chars-1]; 
    mbstate_t state;
    memset(&state, 0, sizeof(state));

    // Convert each utf-8 code point to utf-32 code points
    for (size_t i=0; i<utf8_chars; i++)
    {

        utf8_index_temp = mbrtoc32(&(utf32_string[i]), &(stringToConvert->string_content[utf8_index]),
        (size_t) (string_end - utf32_string[i]) , &state);
        if (utf8_index_temp > 0) utf8_index += utf8_index_temp;
    }

    // return the converted string
    SU32_mutable* converted_string = stringU32_create(utf32_string, utf8_chars);
    return converted_string;
}   

SU8_immutable* SU32_ConvertEncoding(SU32_mutable* stringToConvert)
{
    // return the converted string
    mbstate_t state;
    memset(&state, 0, sizeof(state));

    char* utf8_string = malloc(stringToConvert->length_used * sizeof(char32_t));
    size_t utf8_index = 0;

    // Convert utf32 code points to utf8 code points
    for (size_t i=0; i<stringToConvert->length_used; i++)
    {
        utf8_index += c32rtomb(&utf8_string[utf8_index], stringToConvert->string_content[i],
            &state);
    }

    utf8_string[utf8_index] = '\0';    

    // Create a new string object and return it
    SU8_immutable* converted_string = stringU8_create_conversion(utf8_string, 
        stringToConvert->length_used, utf8_index+1);
    return converted_string;
}


void SU8_bitPrint(char byteToPrint, char* outputName)
{
    char oneBitMask = 0b0000001;
    char result;
    printf("%s: ", outputName);
    for (char i=7; i>-1; --i)
    {
        result = oneBitMask & (byteToPrint >> i);
        printf("%d", result);
    }
    printf("\n");
}


int main()
{
    setlocale(LC_ALL, "en_US.utf8");
    
    SU8_immutable* string = stringU8_create("ååååååååå♥♥♥😂😂😂");

    SU32_mutable* mutString = SU8_ConvertEncoding(string);

    SU8_immutable* string2 = SU32_ConvertEncoding(mutString);

    for (size_t i=0; i<string->multibyte_chars_total; ++i)
        printf("0x%08X ", string->string_content[i]);
    printf("\n");
    for (size_t i=0; i<mutString->length_used; ++i)
        printf("0x%08X ", mutString->string_content[i]);
    printf("\n");

    wprintf(L"%s\n", string->string_content);
    wprintf(L"%s\n", string2->string_content);
    
}