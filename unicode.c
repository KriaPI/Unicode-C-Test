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
    size_t      length_total;
    size_t      size_allocated;
    bool        initialized;
} 
SU8_immutable;

SU8_immutable stringU8_create(char* stringContent)
{
    size_t allocationSize = strlen(stringContent);
    
}



void stringU8_ConvertEncoding(SU8_immutable* stringToConvert, SU32_mutable* destination)
{
    // Figure out how to store a string of mutlibyte characters in a 
    // char32_t string
    size_t string_length = stringToConvert->length_total;

    destination->length_total = stringToConvert->length_total;
    destination->string_content = malloc(string_length * sizeof(char32_t) );
    destination->size_allocated = string_length * sizeof(char32_t);

    mbstate_t shiftState = 0;
    
    for (size_t i=0; i<string_length; i++)
    {
        mbrtoc32(destination->string_content, stringToConvert->string_content,
                 stringToConvert->size_allocated, &shiftState);
    }
}

int main()
{
    setlocale(LC_ALL, "en_US.utf8");


    const char* mbstr = u8"🤢😷 🤒 🤕 🤑ö"; 
    
    const char32_t u32str = U"🤢😷 🤒 🤕 🤑";
    wprintf(U"String: %ls", u32str);
    wprintf(L"MB string: %s\n", mbstr);
    
    
}