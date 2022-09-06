#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>
#include <locale.h>
#include <stdint.h>
#include <stdbool.h>

// note: mblen is not thread-safe!

//enum {};

typedef struct stringU8
{
    const wchar_t* content;
    size_t length_used;
    size_t length_allocated;
    bool initialized;

} stringU8;

void stringU8_create()
{

}

int main()
{
    setlocale(LC_ALL, "en_US.utf8");
    const char* mbstr = u8"🤢😷 🤒 🤕 🤑ö"; 
    wchar_t* wstr;

    // Replace number literal 30 with a length variable. 

    mbstowcs(wstr, mbstr, 30);
    wprintf(L"MB string: %s\n", mbstr);
    wprintf(L"Wide string: %ls\n", wstr);
    
}