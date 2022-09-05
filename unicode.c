#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>
#include <locale.h>



int main()
{
    setlocale(LC_ALL, "en_US.utf8");
    const char* mbstr = u8"🤢😷 🤒 🤕 🤑"; 
    wprintf(L"MB string: %s\n", mbstr);
    

}