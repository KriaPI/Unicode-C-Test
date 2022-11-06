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

void PSL_bitPrint_char(char byteToPrint, char* outputName)
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
void PSL_bitPrint_c32(char32_t byteToPrint, char* outputName)
{
    char32_t oneBitMask = (char32_t) 0b0000000000000000000000000001;
    char result;
    wprintf(L"%s: ", outputName);
    for (char i=31; i>-1; --i)
    {
        result = oneBitMask & (byteToPrint >> i);
        wprintf(L"%d", result);
    }
    wprintf(L"\n");
}

char32_t c8_to_c32(char* utf_8_character, size_t bytes) {
	
	// Create a utf-32 character from 1-4 utf-8 code points. 
    // cast char to char32_t shift bits by 8 (<<) and mask off
    // all bits except the initial first byte (the one shifted).
    // Combine all code points by using OR.  

    char32_t c32 = 0;
    char32_t one_byte_mask = 0xff;
    size_t bytes_consumed = 0;
    size_t i = bytes - 1;

    while (bytes_consumed != bytes)
    {
        char32_t c32_sub_point = (char32_t) utf_8_character[i] << (8 * (bytes - i - 1));
        c32_sub_point &= (one_byte_mask << (8 * (bytes - i - 1)));
        c32 = c32 | c32_sub_point;
        --i;
        ++bytes_consumed;
    } 

    return c32;
}


int main()
{
	setlocale(LC_ALL, "en_US.utf8");
	char* a = "😂";
	char32_t a32 = '😂';

	char32_t b32 = c8_to_c32(a, (size_t) 4);


	PSL_bitPrint_c32(a32, "UTF-32  ");
	PSL_bitPrint_c32(b32, "UTF-32 c");
    
	return 0;
} 