#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>
#include <locale.h>
#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>
 

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
    size_t bytes = strlen(stringMB);
    SU8_immutable* object = malloc(sizeof(SU8_immutable)); 

    *object = (SU8_immutable) 
    {
        .string_content = stringMB,
        .multibyte_chars_total = SU8_multibyte_length(stringMB, bytes),
        .size_allocated = bytes,
        .initialized = true
    };

    return object;
}



void stringU8_ConvertEncoding(SU8_immutable* stringToConvert, SU32_mutable* destination)
{
    // Figure out how to store a string of mutlibyte characters in a 
    // char32_t string
    /*size_t string_length = stringToConvert->length_total;

    destination->length_total = stringToConvert->length_total;
    destination->string_content = malloc(string_length * sizeof(char32_t) );
    destination->size_allocated = string_length * sizeof(char32_t);

    mbstate_t shiftState = 0;
    size_t byteOffset = 0;
    char* end = (stringToConvert->string_content) + (stringToConvert->size_allocated);

    for (size_t i=0; i<string_length; i++)
    {
        byteOffset += mbrtoc32((&destination->string_content[i]), 
                                stringToConvert->string_content[byteOffset], 
                                end - stringToConvert->string_content[byteOffset], 
                                &shiftState);
        
    }*/
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


    //const char* mbstr = u8"🤢😷 🤒 🤕 🤑ö"; 
    
    //stringU8_ConvertEncoding()
    // NOTE: utf-32 strings cannot be and need to be converted to utf-8 strings. 
    //wprintf(L"MB string: %s\n", mbstr);
    
    const char* stringToTest = "åäö🤢😷";
    
    size_t bytes = strlen(stringToTest);
    size_t stringLength = SU8_multibyte_length(stringToTest, bytes);
    printf("stringLength: %lld\n", stringLength);

}