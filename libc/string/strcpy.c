#include <string.h>

char* strcpy(char* dest, const char* src)
{
    if (dest == NULL || src == NULL) {
        return dest;
    }
    char* ret = dest;
    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';

    return ret;
}