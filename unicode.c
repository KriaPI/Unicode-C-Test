#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>
#include <locale.h>
#include <stdint.h>
#include <stdbool.h>
#define __STD_UTF_8__
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
    char16_t* string_content;
    size_t    length_used;
    size_t    length_total;
    size_t    size_allocated;
    bool      initialized;
} 
SU16_immutable;

typedef struct
{
    const char* string_content;
    size_t      multibyte_chars_total;
    size_t      size_allocated;
    bool        initialized;
} 
SU8_immutable;

size_t PSL_StringLength_u8(const char* stringMB, size_t bytes)
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

SU8_immutable* PSL_createString_u8(const char* stringMB)
{
    // Store a string literal within the struct SU8_immutable
    // and populate all struct members with a designated initialization.
    size_t bytes          = strlen(stringMB);
    SU8_immutable* object = malloc(sizeof(SU8_immutable)); 

    *object = (SU8_immutable) 
    {
        .string_content        = stringMB,
        .multibyte_chars_total = PSL_StringLength_u8(stringMB, bytes)+1,
        .size_allocated        = bytes,
        .initialized           = true
    };

    return object;
}

SU8_immutable* PSL_createConversion_u8(const char* stringMB, size_t characters, size_t bytes)
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

SU32_mutable* PSL_createString_u32(char32_t* string, size_t characters)
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

SU16_immutable* PSL_createString_u16(char16_t* string, size_t characters)
{
    // Store a string literal within the struct SU8_immutable
    // and populate all struct members with a designated initialization.
    SU16_immutable* object = malloc(sizeof(SU16_immutable)); 

    *object = (SU16_immutable) 
    {
        .string_content = string,
        .length_used    = characters,
        .size_allocated = characters * sizeof(char16_t),
        .initialized    = true
    };

    return object;
}

void PSL_bitPrint(char byteToPrint, char* outputName)
{
    char oneBitMask = 0b0000001;
    char result;
    wprintf(L"%s: ", outputName);
    for (char i=7; i>-1; --i)
    {
        result = oneBitMask & (byteToPrint >> i);
        wprintf(L"%d", result);
    }
    wprintf(L"\n");
}

void PSL_equal_check(SU8_immutable* string1, SU8_immutable* string2)
{
    for (size_t i=0; i<string1->size_allocated; i++)
    {
        if (string1->string_content[i] != string2->string_content[i])
        {
            wprintf(L"Not Equal\n");
            return;
        }
    }
    wprintf(L"Equal\n");
}

int main()
{
    setlocale(LC_ALL, "en_US.utf8");
    
    SU8_immutable* string = PSL_createString_u8("ååååååååå♥♥♥😂😂😂");
    SU32_mutable* string_converted = PSL_utf8_to_utf32(string);
    SU8_immutable* string2 = PSL_utf32_to_utf8(string_converted);

    //wprintf(L"%s", realString);
    //wprintf(L"%ls\n", s16->string_content);
    PSL_equal_check(string, string2);
    wprintf(L"%s\n", string->string_content);
    wprintf(L"%s\n", string2->string_content);
}