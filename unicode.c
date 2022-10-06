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


// Tested, does work. 
SU32_mutable* PSL_utf8_to_utf32(SU8_immutable* stringToConvert)
{
    size_t utf8_chars        = stringToConvert->multibyte_chars_total;
    size_t utf32_string_size = utf8_chars * sizeof(char32_t);
    size_t utf8_index        = 0;
    size_t utf8_index_temp   = 0;

    char32_t* utf32_string = malloc(utf32_string_size);
    char32_t* string_end   = &utf32_string[utf8_chars]; 
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
    SU32_mutable* converted_string = PSL_createString_u32(utf32_string, utf8_chars);
    return converted_string;
}

SU16_immutable* PSL_utf8_to_utf16(SU8_immutable* string)
{
    size_t u8_chars = string->multibyte_chars_total;
    size_t u16_allocation_size = u8_chars * sizeof(char16_t);

    char16_t* u16_string = malloc(u16_allocation_size);


    size_t conversion_result = 0;
    size_t processed_bytes = 0;
    size_t i = 0;

    mbstate_t state;
    memset(&state, 0, sizeof(state));
    
    size_t bytes_to_process = 0;
    while (processed_bytes < string->size_allocated)
    {
        bytes_to_process = (string->size_allocated - processed_bytes);


        conversion_result = mbrtoc16(
            &(u16_string[i]), 
            &(string->string_content[processed_bytes]), 
            bytes_to_process,
            &state
        );

        if (conversion_result > (size_t) 0) processed_bytes += conversion_result;
        i++;

    } 

    SU16_immutable* convertedString = PSL_createString_u16(u16_string, i);
    return convertedString;
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


int main()
{
    setlocale(LC_ALL, "en_US.utf8");
    
    SU8_immutable* string = PSL_createString_u8("ååååååååå♥♥♥😂😂😂");
    SU16_immutable* s16 = PSL_utf8_to_utf16(string);

    /*
    printf("utf-8:\n");
    for (size_t i=0; i<string->multibyte_chars_total; ++i)
        printf("0x%08X ", string->string_content[i]);
    printf("\n");
    
    printf("utf-16:\n");
    for (size_t i=0; i<s16->length_used; ++i)
        printf("0x%08X ", s16->string_content[i]);
    printf("\n");
    */

    
    //wprintf(L"%s", realString);
    wprintf(L"%ls\n", s16->string_content);
    wprintf(L"%s\n", string->string_content);
    
}