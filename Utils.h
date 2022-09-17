#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef int_32_t
    typedef unsigned long uint_64_t;
    typedef long int_64_t;
    typedef unsigned int uint_32_t;
    typedef int int_32_t;
    typedef unsigned short uint_16_t;
    typedef short int_16_t;
#endif

#ifndef int_32
    #define uint_64 uint_64_t
    #define int_64 int_64_t
    #define uint_32 uint_32_t
    #define int_32 int_32_t
    #define uint_16 uint_16_t
    #define int_16 int_16_t
    #define uint_8 uint_8_t
    #define int_8 uint_8_t
#endif

#ifndef bool
    #define true 1
    #define false 0
    
    typedef uint_16 bool;
#endif

#ifndef _WIN32
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    
    #define Sleep sleep
    
    typedef int_32 Socket;
#else
    #include <windows.h>
    #include <winsock2.h>
    
    #define Sleep(s) sleep((int_32) ((s) * 1000))
#endif

#define DEFAULT_SLEEP 0.05

void wait(float ms)
{
    if(ms < 0)
    {
        fprintf(stderr, "Wait provided negative value: %f", ms);
        ms *= -1;
    }
    
    Sleep(ms);
}

bool isnumber(const char c) {return (c >= '0' && c <= '9');}

bool isnumeric(const char* str, const size_t length)
{
    for(size_t i = 0; i < length; i++) if(!isnumber(*(str + i))) return false;
    return true;
}

//bool isWhitespace(const char c) {return (c == '​' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == '⠀');}

size_t substring(char* dest, const char* src, const size_t begin, const size_t end, const size_t dest_size)
{
    size_t src_len = (end < strlen(src)) ? end : strlen(src);
    
    char* temp = (char*) malloc(dest_size * sizeof(char));
    
    size_t i = 0;
    
    for(i = begin; i < src_len; i++)
    {
        if((i - begin) >= dest_size) break;
        *(temp + (i - begin)) = *(src + i);
    }
    
    *(temp + ++i) = '\0';
    
    snprintf(dest, dest_size, "%s", temp);
    free(temp);
    
    return strlen(dest);
}

void stradd(char* dest, const char* src, const size_t index, const size_t dest_size)
{
    if(dest_size < (strlen(dest) + strlen(src)) || index > dest_size)
    {
        fprintf(stderr, "Error: stradd cannot fit \"%s\" into destination \"%s\" at index %ld with maximum size %ld.", src, dest, index, dest_size);
        return;
    }
    
    if(strlen)
    
    const size_t dest_len = (strlen(src) + strlen(dest));
    char* temp = (char*) malloc(dest_size * sizeof(char));
    
    
}

int_32 indexOf(const char* string, const char c, const size_t length)
{
    for(size_t i = 0; i < length; i++)
    {
        if(i > INT_MAX)
        {
            fprintf(stderr, "Error: indexOf overflow searching for \'%c\' in \"%s\".", c, string);
            return -1;
        }
        
        if(*(string + i) == c) return i;
    }
    
    return -1;
}

int_32 lastIndexOf(const char* string, const char c, const size_t length)
{
    if(length > INT_MAX)
    {
        fprintf(stderr, "Error: lastIndexOf overflow searching for \'%c\' in \"%s\".", c, string);
        
        return -1;
    }
    
    for(size_t i = length; i >= 0; i--) if(*(string + i) == c) return i;
    
    return -1;
}

int_32 nextIndexOf(const char* string, const char c, const size_t index, const size_t length)
{
    if(index >= length)
    {
        fprintf(stderr, "Error: nextIndexOf index [%ld] is greater than or matches length %ld.\n", index, length);
        return -1;
    }
    
    const size_t dest_len = (length - index);
    char* temp = (char*) malloc(dest_len * sizeof(char));
    
    substring(temp, string, index, length, dest_len);
    int_32 next_index = indexOf(temp, c, dest_len);
    
    if(next_index != -1) next_index += index;
    
    free(temp);
    return next_index;
}

int_32 prevIndexOf(const char* string, const char c, const size_t index, const size_t length)
{
    if(index >= length)
    {
        fprintf(stderr, "Error: prevIndexOf index [%ld] is greater than or matches length %ld.\n", index, length);
        return -1;
    }
    
    int_32 prev_index = lastIndexOf(string, c, index);
    
    if(prev_index == index) prev_index = lastIndexOf(string, c, (index - 1));
    
    return prev_index;
}

void strtoken(char* dest, const char* src, const char seperator, size_t* index, const size_t dest_size)
{
    const size_t src_len = strlen(src);
    if(*index >= src_len || *index < 0)
    {
        fprintf(stderr, "Error: Index [%ld] is greater then source length of %ld.\n", *index, src_len);
        return;
    }
    
    if(*(src + *index) == seperator)
    {
        fprintf(stderr, "Error: strtoken found seperator [%c] at index [%ld] in string \"%s\".", seperator, *index, src);
        
        *index += 1;
        strtoken(dest, src, seperator, index, dest_size);
        return;
    }
    
    char* temp = (char*) malloc(dest_size * sizeof(char));
    
    int_32 end = nextIndexOf(src, seperator, *index, src_len);
    
    if(end == -1) substring(temp, src, *index, src_len, dest_size);
    else substring(temp, src, *index, end, dest_size);
    
    snprintf(dest, dest_size, "%s", temp);
    
    if(end == -1) *index = strlen(src);
    else *index = ++end;
    free(temp);
}
