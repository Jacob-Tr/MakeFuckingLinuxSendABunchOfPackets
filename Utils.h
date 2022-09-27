#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// ------------CRSS_TYPE_HEADER------------

#ifndef CRSS_TYPE_H
#define CRSS_TYPE_H

#ifdef __cplusplus
extern "C"
{
#endif

const size_t __CHAR_PTR_SIZE__ = (sizeof(char*));

#if (__CHAR_PTR_SIZE__ == 1)
    #define __8BIT__
#elif (__CHAR_PTR_SIZE__ == 2)
    #define __16BIT__
#elif (__CHAR_PTR_SIZE__ == 4)
    #define __32BIT__
#else
    #define __64BIT__
#endif

#ifndef byte
    #define byte char
#endif

#if !defined uint_8_t && !defined uint_8 && defined uint
    #ifdef __64BIT__
        //typedef ulong long ulonglong;
    
        typedef ulonglong unsigned long long;
    #endif
    #if defined __64BIT__ || defined __32BIT__
        typedef ulong unsigned long;
    #endif
    typedef uint unsigned int;
    #if defined __64BIT__ || defined __32BIT__
        typedef ushort unsigned short;
    #endif
    typedef uchar unsigned char;
    #ifndef __8BIT__
        typedef ubyte unsigned byte;
    #endif
#endif

#ifndef int_8_t
    #ifdef __64BIT__
        typedef unsigned long long uint_128_t;
        typedef long long int_128_t;
    #endif
    #if defined __64BIT__ || defined __32BIT__
        typedef unsigned long uint_64_t;
        typedef long int_64_t;
        typedef unsigned int uint_32_t;
        typedef int int_32_t;
        typedef unsigned short uint_16_t;
        typedef short int_16_t;
    #else
        #ifdef __16BIT__
            typedef unsigned long uint_32_t;
            typedef long int_32_t;
            typedef unsigned int uint_16_t;
            typedef int int_16_t;
        #else
            typedef unsigned long uint_16_t;
            typedef long int_16_t;
            typedef unsigned int uint_8_t;
            typedef int int_8_t;
        #endif
    #endif
    #ifndef __8BIT__
        typedef unsigned byte uint_8_t;
        typedef byte int_8_t;
    #endif
#endif

#ifndef int_8
    #if defined __64BIT__ || defined __32BIT__ || defined __16BIT__
        #ifdef __64BIT__
            #define uint_128 uint_128_t
            #define int_128 int_128_t
        #endif
        #if !defined __16BIT__ && !defined __8BIT__
            #define uint_64 uint_64_t
            #define int_64 int_64_t
            #define uint_32 uint_32_t
            #define int_32 int_32_t
        #endif
        #ifndef __8BIT__
            #define uint_16 uint_16_t
            #define int_16 int_16_t
        #endif
    #endif
    #define uint_8 uint_8_t
    #define int_8 uint_8_t
#endif

#ifndef bool
    #define true 1
    #define false 0
    
    typedef uint_8 bool;
#endif

#if !defined string && !defined String
    typedef struct
    {
        char* content;
        size_t length;
    } String;
    
    #define string(content) initString(content)
    String* initString(const char* content)
    {
        String* strct = (String*) malloc(sizeof(String));
        char* cont = (char*) malloc(sizeof(char*) * strlen(content));
        
        snprintf(cont, strlen(content), "%s", content);
        
        strct->length = strlen(cont);
        strct->content = cont;
        
        return strct;
    }
    
    void strset(String str, const char* content)
    {
        if(strlen(content) > str.length) str.content = (char*) realloc(str.content, strlen(content) * sizeof(char));
        else
        {
            free(str.content);
            str.content = (char*) malloc(strlen(content) * sizeof(char));
        }
        
        snprintf(str.content, strlen(content), "%s", content);
    }
    
    void strclr(String str)
    {
        free(str.content);
        str.length = 0;
    }
    
    void strfree(String* str)
    {
        free(str->content);
        free(str);
    }
    
    char* str(const String str) {return str.content;}
    
    size_t strlength(const String str) {return str.length;}
#endif

#ifdef __cplusplus
}
#endif

// ----------END_CRSS_TYPE_HEADER-----------

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
#endif

// ------------UTILS_HEADER-----------

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus 
extern "C"
{
#endif

#ifdef _WIN32 
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

bool substring(char* dest, const char* src, const size_t begin, const size_t end, const size_t dest_size)
{
    if(dest_size < ((end + 1) - begin) || begin >= end || end > strlen(src))
    {
        fprintf(stderr, "Error: Substring could not extract index %ld to %ld of string \"%s\" into destination of provided size %ld.\n", begin, end, src, dest_size);
        
        return false;
    }

    snprintf(dest, ((end + 1) - begin), "%s", (src + begin));
    
    return true;
}

void stradd(char* dest, const char* src, const size_t index, const size_t dest_size)
{
    if(dest_size < (strlen(dest) + strlen(src)) || index > dest_size)
    {
        fprintf(stderr, "Error: stradd cannot fit \"%s\" into destination \"%s\" at index %zu with maximum size %zu.\n", src, dest, index, dest_size);
        return;
    }

    const size_t dest_len = (strlen(src) + strlen(dest));
    char* end_temp = "\0", *temp = (char*) malloc(dest_len * sizeof(char));;

    if(strlen(dest) < index) for(size_t i = 0; i < index; i++) *(dest + i) = ' ';
    else if((index + 1) < strlen(dest))
    {
        end_temp = (char*) malloc((dest_len - index) * sizeof(char));
        substring(end_temp, dest, (index + 1), strlen(dest), (dest_len - index));
    }

    substring(temp, dest, 0, strlen(dest), dest_size);
    for(size_t i = index; i < dest_len; i++)
    {
        if((i - index) < strlen(src)) *(temp + i) = *(src + (i - index));
        else *(temp + i) = *(end_temp + (i - (strlen(src) + index)));
    }

    snprintf(dest, dest_len, "%s", temp);

    free(end_temp);
    free(temp);
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

// NEW ADDITIONS

bool strfind(const char* string, const char* substring, int_32* begin, int_32* end, const size_t sub_len, const size_t length)
{
    if((length < sub_len) || (length == 0))
    {
        *begin = -1;
        *end = -1;
        
        return false;
    }
    
    char current = '\0', sub_cur = *(substring);
    *begin = indexOf(string, sub_cur, length);
    
    for(size_t i = *begin; i < length; i++)
    {
        current = *(string + i);
        
        if(*begin == -1)
        {
            if(*(string + i) == *(substring)) *begin = i;
            continue;
        }
        
        sub_cur = *(substring + (i - *begin));
        
        if(sub_cur != current)
        {
            *begin = -1;
            continue;
        }
        else if((*begin - i) >= sub_len)
        {
            *end = (i + 1);
            break;
        }
    }
    
    return (*begin >= 0 && *end >= 1);
}

bool strdel(char* dest, const char* src, const size_t begin, const size_t end, const size_t length)
{
    if(begin >= end || end > length)
    {
        fprintf(stderr, "Error: strdel could not remove substring index %ld to %ld from \"%s\".\n", begin, end, src);
        
        return false;
    }
    
    char* new_str = (char*) malloc((length - (end - begin)) * sizeof(char));
    
    snprintf(new_str, (begin + 1), "%s", src);
    snprintf((new_str + (begin)), length, "%s", (src + end));
    
    snprintf(dest, length, "%s", new_str);
    
    free(new_str);
    
    return true;
}

bool strContainsChar(const char* string, const char ch, const size_t length) {return (indexOf(string, ch, length) != -1);}

bool strContains(const char* string, const char* string2, const size_t length, const size_t length2)
{
    if(length2 > length) return false;
    
    char ch = *string2;
    int_32 index = (indexOf(string, ch, length) + 1), found = 1;
    
    if(index == 0) return false;
    
    for(size_t i = index; i < length; i++)
    {
        if(found == length2) return true;
        if(i >= (length2 - 1)) return false;
        
        if(*(string + i) != *(string2 + (i - index))) return strContains((string + i), string2, (length - i), length2);
    }
    
    if(found == length2) return true;
    return false;
}

bool strMatch(const char* string, const char* string2, const size_t length, const size_t length2) 
{
    if(length != length2) return false;
    return (strncmp(string, string2, length) == 0);
}

#ifdef __cplusplus 
}
#endif

#endif
// --------------END_UTILS_HEADER-----------

// ----------------FILE_HEADER--------------
#ifndef FILE_H
#define FILE_H

typedef enum
{
   FILE_DATA,
   FILE_TYPES
} ftype;

typedef struct
{
    char* path;
    size_t path_len;
    ftype type;
    FILE* file;
    bool open;
    size_t length;
} file;
#define invalid_file (file*) NULL
uint_32 files = 0;

char* file_ext[FILE_TYPES] =
{
    ".ini"
};

#define MAX_DIS_CHARS 9 

char disallowed_chars[] = {':', '*', 	'?', 	'"', 	'<', 	'>', 	'|'}; 

#ifdef __WIN32

#define MAX_DIS_NAMES 24 
char disallowed_names[MAX_DIS_NAMES][5] = 
{ 	
    "CON", 
    	"PRN", 
    	"AUX", 
    	"NUL", 
    	"COM1", 	
    "COM2", 	
    "COM3", 
    	"COM4", 
    	"COM5", 
    	"COM6", 
    	"COM7", 
    	"COM8", 	
    "COM9",
    	"COM0",
    	"LPT1", 	
    "LPT2", 
    	"LPT3", 
    	"LPT4", 
    	"LPT5", 
    	"LPT6", 	
    "LPT7", 	
    "LPT8", 
    	"LPT9", 
    	"LPT0" 
};

#endif

#ifdef __cplusplus
extern "C"
{
#endif

bool doesStrContainDisallowedChars(const char* string) 
{ 	
    for(size_t i = 0; i <= MAX_DIS_CHARS; i++) if(strContainsChar(string, disallowed_chars[i], strlen(string))) return true;
    
    return false; 
} 

bool isValidWinFileName(const char* string) 
{
#ifdef _WIN32
    if(doesStrContainDisallowedChars(string)) return false;
    
    for(size_t i = 0; i < MAX_DIS_NAMES; i++)
    {
        length = strlen(disallowed_names[i]);
        if(strmatch(string, disallowed_names[i], strlen(string), strlen(disallowed_names[i]))) return false;
    }
#endif
    return true; 
} 

bool isValidWinDir(const char* string)
{
    if(strlen(string) < 3) return false;

    char* disk = (char*) malloc(3 * sizeof(char));
    
    snprintf(disk, 3, "%s", string);
    
    if((*disk > 'Z' || *disk < 'A') || *(disk + 1) != ':' || *(disk + 2) != '/') 
    {
        free(disk);
        return false;
    }
    
    free(disk);
    return true;
}

bool isValidDirectory(const char* string) 
{ 
    if(doesStrContainDisallowedChars(string)) return false; 
    
#ifdef _WIN32
    return isValidWinDir(string);
#endif

    return true; 
}

bool isValidFileName(const char* string, const size_t length)
{
#ifdef _WIN32
    if(!isValidWinFileName(string)) return false;
#endif

    if(doesStrContainDisallowedChars(string)) return false;
    
    if(strContainsChar(string, '/', length) || strContainsChar(string, '\\', length)) return false;
    
    return true;
}

bool isValidFileType(const ftype type) {return (type < 0 || type > FILE_TYPES);}

char* getFileTypeExtension(const ftype type) {
    if(!isValidFileType(type)) return "";
    return file_ext[type];
}

#define File(name, path, type, name_len, path_len) initFile((name), (path), (type), (name_len), (path_len))
file* initFile(const char* path, const char* name, const ftype type, const size_t path_len, const size_t name_len)
{
    if(path_len == 0 || name_len == 0)
    {
        fprintf(stderr, "Error: initFile could initialize file with path length %zu and file name length %zu.\n", path_len, name_len);
        
        return invalid_file;
    }
    
    if(!isValidFileType(type))
    {
        fprintf(stderr, "Error: initFile could initialize file with invalid file type %d.\n", (uint_32) type);
        
        return invalid_file;
    }
    
    if(!isValidDirectory(path))
    {
        fprintf(stderr, "Error: initFile could initialize file with invalid path \"%s\".\n", path);
        
        return invalid_file;
    }
    
    if(!isValidFileName(name, strlen(name)))
    {
        fprintf(stderr, "Error: initFile could initialize file with invalid file name \"%s\".\n", name);
        
        return invalid_file;
    }
    
    char* xtnsn = getFileTypeExtension(type);
    const size_t length = ((path_len + name_len + strlen(xtnsn)) + 1);
    char* f_name = (char*) malloc(length * sizeof(char));
    
    snprintf(f_name, length, "%s/%s%s", path, name, xtnsn);
    
    FILE* f = NULL;
    f = fopen(f_name, "R");
    
    if(f == NULL)
    {
        fprintf(stderr, "Error: initFile could not create a file stream for file \"%s\".\n", f_name);
        
        free(f);
        free(f_name);
        return invalid_file;
    }
    
    file* file_strct = (file*) malloc(sizeof(file));
    
    file_strct->path = f_name;
    file_strct->path_len = length;
    file_strct->file = f;
    file_strct->type = type;
    file_strct->open = false;
    file_strct->length = fseek(file_strct->file, 0, SEEK_END);
    
    fseek(file_strct->file, 0, SEEK_SET);
    fclose(f);
    
    files++;
    
    return file_strct;
}

bool isFileOpen(const file f) {return (f.open);}

char* getFileName(const file f) {return f.path;}

size_t getFileNameLen(const file f) {return f.length;}

ftype getFileType(const file f) {return f.type;}

bool FOpen(file* f, const char* mode) 
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FOpen called on invalid file.\n");
        return false;
    }

    if(isFileOpen(*f))
    {
        fprintf(stderr, "Error: FOpen called on already open file: \"%s\".\n", f->path);
        
        return false;
    }
    
    f->file = freopen(f->path, mode, f->file);
    
    if(f->file == NULL)
    {
        fprintf(stderr, "Error: FOpen has Insufficient permissions to open \"%s\" with filemode \"%s\" or file not found.\n", f->path, mode);
        
        return false;
    }
    
    f->open = true;
    return true;
}

bool FClose(file* f)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FClose called on invalid file.\n");
        return false;
    }
    
    if(!isFileOpen(*f))
    {
        fprintf(stderr, "Error: FClose cannot close already closed file: \"%s\".\n", f->path);
        return false;
    }
    
    fclose(f->file);
    f->open = false;
    
    return true;
}

size_t FGetSize(file* f)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FGetSize called on invalid file.\n");
        return 0;
    }
    
    bool close = false;
    if(!isFileOpen(*f))
    {
        if(FOpen(f, "R")) close = true;
        else
        {
            fprintf(stderr, "Error: FGetSize called on closed file: \"%s\" which failed to open.\n", f->path);
        
            return 0;
        }
    }
    
    size_t size = fseek(f->file, 0, SEEK_END);
    fseek(f->file, 0, SEEK_SET);
    
    if(close) FClose(f);
    
    return size;
}


bool FRead(char* dest, const file* f, const size_t dest_size)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FRead called on invalid file.\n");
        return false;
    }
    
    if(!isFileOpen(*f))
    {
        fprintf(stderr, "Error: FRead called on closed file: \"%s\".\n", f->path);
        return false;
    }
    
    fseek(f->file, 0, SEEK_SET);
    fread(dest, sizeof(char), dest_size, f->file);
    
    return true;
}

bool FWrite(file* f, const char* content, const size_t length)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FWrite called on invalid file.\n");
        return false;
    }
    
    if(!isFileOpen(*f))
    {
        fprintf(stderr, "Error: FWrite called on closed file: \"%s\".\n", f->path);
        return false;
    }
    
    f->length = fwrite(content, length, sizeof(char), f->file);
    fflush(f->file);
    
    return true;
}

void FDestruct(file* f)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FDestruct called on invalid file.\n");
        return;
    }
    
    if(isFileOpen(*f)) FClose(f);
    
    free(f->path);
    
    file* temp = f;
    f = invalid_file;
    
    free(temp);
    
    files--;
}

#ifdef __cplusplus
}
#endif

#endif

// --------------END_FILE_HEADER------------


#define TEST_SIZE ((size_t) 128)

int main() 
{
    char* str = (char*) malloc(TEST_SIZE * sizeof(char)), *str2 = (char*) malloc(TEST_SIZE * sizeof(char));
    snprintf(str, TEST_SIZE, "-c-a-b-x-3");
    
    int_32 begin = 0, end = 3;
    
    //if(strdel(str2, str, begin, end, strlen(str))) 
    
    //snprintf((str + 3), strlen(str), "LOL");
    
    substring(str2, str, begin, end, TEST_SIZE);
    
    fprintf(stdout, "Str2: \"%s\".\n", str2);
    return 0;
}#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// ------------CRSS_TYPE_HEADER------------

#ifndef CRSS_TYPE_H
#define CRSS_TYPE_H

#ifdef __cplusplus
extern "C"
{
#endif

const size_t __CHAR_PTR_SIZE__ = (sizeof(char*));

#if (__CHAR_PTR_SIZE__ == 1)
    #define __8BIT__
#elif (__CHAR_PTR_SIZE__ == 2)
    #define __16BIT__
#elif (__CHAR_PTR_SIZE__ == 4)
    #define __32BIT__
#else
    #define __64BIT__
#endif

#ifndef byte
    #define byte char
#endif

#if !defined uint_8_t && !defined uint_8 && defined uint
    #ifdef __64BIT__
        //typedef ulong long ulonglong;
    
        typedef ulonglong unsigned long long;
    #endif
    #if defined __64BIT__ || defined __32BIT__
        typedef ulong unsigned long;
    #endif
    typedef uint unsigned int;
    #if defined __64BIT__ || defined __32BIT__
        typedef ushort unsigned short;
    #endif
    typedef uchar unsigned char;
    #ifndef __8BIT__
        typedef ubyte unsigned byte;
    #endif
#endif

#ifndef int_8_t
    #ifdef __64BIT__
        typedef unsigned long long uint_128_t;
        typedef long long int_128_t;
    #endif
    #if defined __64BIT__ || defined __32BIT__
        typedef unsigned long uint_64_t;
        typedef long int_64_t;
        typedef unsigned int uint_32_t;
        typedef int int_32_t;
        typedef unsigned short uint_16_t;
        typedef short int_16_t;
    #else
        #ifdef __16BIT__
            typedef unsigned long uint_32_t;
            typedef long int_32_t;
            typedef unsigned int uint_16_t;
            typedef int int_16_t;
        #else
            typedef unsigned long uint_16_t;
            typedef long int_16_t;
            typedef unsigned int uint_8_t;
            typedef int int_8_t;
        #endif
    #endif
    #ifndef __8BIT__
        typedef unsigned byte uint_8_t;
        typedef byte int_8_t;
    #endif
#endif

#ifndef int_8
    #if defined __64BIT__ || defined __32BIT__ || defined __16BIT__
        #ifdef __64BIT__
            #define uint_128 uint_128_t
            #define int_128 int_128_t
        #endif
        #if !defined __16BIT__ && !defined __8BIT__
            #define uint_64 uint_64_t
            #define int_64 int_64_t
            #define uint_32 uint_32_t
            #define int_32 int_32_t
        #endif
        #ifndef __8BIT__
            #define uint_16 uint_16_t
            #define int_16 int_16_t
        #endif
    #endif
    #define uint_8 uint_8_t
    #define int_8 uint_8_t
#endif

#ifndef bool
    #define true 1
    #define false 0
    
    typedef uint_8 bool;
#endif

#if !defined string && !defined String
    typedef struct
    {
        char* content;
        size_t length;
    } String;
    
    #define string(content) initString(content)
    String* initString(const char* content)
    {
        String* strct = (String*) malloc(sizeof(String));
        char* cont = (char*) malloc(sizeof(char*) * strlen(content));
        
        snprintf(cont, strlen(content), "%s", content);
        
        strct->length = strlen(cont);
        strct->content = cont;
        
        return strct;
    }
    
    void strset(String str, const char* content)
    {
        if(strlen(content) > str.length) str.content = (char*) realloc(str.content, strlen(content) * sizeof(char));
        else
        {
            free(str.content);
            str.content = (char*) malloc(strlen(content) * sizeof(char));
        }
        
        snprintf(str.content, strlen(content), "%s", content);
    }
    
    void strclr(String str)
    {
        free(str.content);
        str.length = 0;
    }
    
    void strfree(String* str)
    {
        free(str->content);
        free(str);
    }
    
    char* str(const String str) {return str.content;}
    
    size_t strlength(const String str) {return str.length;}
#endif

#ifdef __cplusplus
}
#endif

// ----------END_CRSS_TYPE_HEADER-----------

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
#endif

// ------------UTILS_HEADER-----------

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus 
extern "C"
{
#endif

#ifdef _WIN32 
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

bool substring(char* dest, const char* src, const size_t begin, const size_t end, const size_t dest_size)
{
    if(dest_size < ((end + 1) - begin) || begin >= end || end > strlen(src))
    {
        fprintf(stderr, "Error: Substring could not extract index %ld to %ld of string \"%s\" into destination of provided size %ld.\n", begin, end, src, dest_size);
        
        return false;
    }

    snprintf(dest, ((end + 1) - begin), "%s", (src + begin));
    
    return true;
}

/*void stradd(char* dest, const char* src, const size_t index, const size_t dest_size)
{
    if(dest_size < (strlen(dest) + strlen(src)) || index > dest_size)
    {
        fprintf(stderr, "Error: stradd cannot fit \"%s\" into destination \"%s\" at index %ld with maximum size %ld.", src, dest, index, dest_size);
        return;
    }
    
    if(strlen)
    
    const size_t dest_len = (strlen(src) + strlen(dest));
    char* temp = (char*) malloc(dest_size * sizeof(char));
    
    
}*/

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

// NEW ADDITIONS

bool strfind(const char* string, const char* substring, int_32* begin, int_32* end, const size_t sub_len, const size_t length)
{
    if((length < sub_len) || (length == 0))
    {
        *begin = -1;
        *end = -1;
        
        return false;
    }
    
    char current = '\0', sub_cur = *(substring);
    *begin = indexOf(string, sub_cur, length);
    
    for(size_t i = *begin; i < length; i++)
    {
        current = *(string + i);
        
        if(*begin == -1)
        {
            if(*(string + i) == *(substring)) *begin = i;
            continue;
        }
        
        sub_cur = *(substring + (i - *begin));
        
        if(sub_cur != current)
        {
            *begin = -1;
            continue;
        }
        else if((*begin - i) >= sub_len)
        {
            *end = (i + 1);
            break;
        }
    }
    
    return (*begin >= 0 && *end >= 1);
}

bool strdel(char* dest, const char* src, const size_t begin, const size_t end, const size_t length)
{
    if(begin >= end || end > length)
    {
        fprintf(stderr, "Error: strdel could not remove substring index %ld to %ld from \"%s\".\n", begin, end, src);
        
        return false;
    }
    
    char* new_str = (char*) malloc((length - (end - begin)) * sizeof(char));
    
    snprintf(new_str, (begin + 1), "%s", src);
    snprintf((new_str + (begin)), length, "%s", (src + end));
    
    snprintf(dest, length, "%s", new_str);
    
    free(new_str);
    
    return true;
}

bool strContainsChar(const char* string, const char ch, const size_t length) {return (indexOf(string, ch, length) != -1);}

bool strContains(const char* string, const char* string2, const size_t length, const size_t length2)
{
    if(length2 > length) return false;
    
    char ch = *string2;
    int_32 index = (indexOf(string, ch, length) + 1), found = 1;
    
    if(index == 0) return false;
    
    for(size_t i = index; i < length; i++)
    {
        if(found == length2) return true;
        if(i >= (length2 - 1)) return false;
        
        if(*(string + i) != *(string2 + (i - index))) return strContains((string + i), string2, (length - i), length2);
    }
    
    if(found == length2) return true;
    return false;
}

bool strMatch(const char* string, const char* string2, const size_t length, const size_t length2) 
{
    if(length != length2) return false;
    return (strncmp(string, string2, length) == 0);
}

#ifdef __cplusplus 
}
#endif

#endif
// --------------END_UTILS_HEADER-----------

// ----------------FILE_HEADER--------------
#ifndef FILE_H
#define FILE_H

typedef enum
{
   FILE_DATA,
   FILE_TYPES
} ftype;

typedef struct
{
    char* path;
    size_t path_len;
    ftype type;
    FILE* file;
    bool open;
    size_t length;
} file;
#define invalid_file (file*) NULL
uint_32 files = 0;

char* file_ext[FILE_TYPES] =
{
    ".ini"
};

#define MAX_DIS_CHARS 9 

char disallowed_chars[] = {':', '*', 	'?', 	'"', 	'<', 	'>', 	'|'}; 

#ifdef __WIN32

#define MAX_DIS_NAMES 24 
char disallowed_names[MAX_DIS_NAMES][5] = 
{ 	
    "CON", 
    	"PRN", 
    	"AUX", 
    	"NUL", 
    	"COM1", 	
    "COM2", 	
    "COM3", 
    	"COM4", 
    	"COM5", 
    	"COM6", 
    	"COM7", 
    	"COM8", 	
    "COM9",
    	"COM0",
    	"LPT1", 	
    "LPT2", 
    	"LPT3", 
    	"LPT4", 
    	"LPT5", 
    	"LPT6", 	
    "LPT7", 	
    "LPT8", 
    	"LPT9", 
    	"LPT0" 
};

#endif

#ifdef __cplusplus
extern "C"
{
#endif

bool doesStrContainDisallowedChars(const char* string) 
{ 	
    for(size_t i = 0; i <= MAX_DIS_CHARS; i++) if(strContainsChar(string, disallowed_chars[i], strlen(string))) return true;
    
    return false; 
} 

bool isValidWinFileName(const char* string) 
{
#ifdef _WIN32
    if(doesStrContainDisallowedChars(string)) return false;
    
    for(size_t i = 0; i < MAX_DIS_NAMES; i++)
    {
        length = strlen(disallowed_names[i]);
        if(strmatch(string, disallowed_names[i], strlen(string), strlen(disallowed_names[i]))) return false;
    }
#endif
    return true; 
} 

bool isValidWinDir(const char* string)
{
    if(strlen(string) < 3) return false;

    char* disk = (char*) malloc(3 * sizeof(char));
    
    snprintf(disk, 3, "%s", string);
    
    if((*disk > 'Z' || *disk < 'A') || *(disk + 1) != ':' || *(disk + 2) != '/') 
    {
        free(disk);
        return false;
    }
    
    free(disk);
    return true;
}

bool isValidDirectory(const char* string) 
{ 
    if(doesStrContainDisallowedChars(string)) return false; 
    
#ifdef _WIN32
    return isValidWinDir(string);
#endif

    return true; 
}

bool isValidFileName(const char* string, const size_t length)
{
#ifdef _WIN32
    if(!isValidWinFileName(string)) return false;
#endif

    if(doesStrContainDisallowedChars(string)) return false;
    
    if(strContainsChar(string, '/', length) || strContainsChar(string, '\\', length)) return false;
    
    return true;
}

bool isValidFileType(const ftype type) {return (type < 0 || type > FILE_TYPES);}

char* getFileTypeExtension(const ftype type) {
    if(!isValidFileType(type)) return "";
    return file_ext[type];
}

#define File(name, path, type, name_len, path_len) initFile((name), (path), (type), (name_len), (path_len))
file* initFile(const char* path, const char* name, const ftype type, const size_t path_len, const size_t name_len)
{
    if(path_len == 0 || name_len == 0)
    {
        fprintf(stderr, "Error: initFile could initialize file with path length %zu and file name length %zu.\n", path_len, name_len);
        
        return invalid_file;
    }
    
    if(!isValidFileType(type))
    {
        fprintf(stderr, "Error: initFile could initialize file with invalid file type %d.\n", (uint_32) type);
        
        return invalid_file;
    }
    
    if(!isValidDirectory(path))
    {
        fprintf(stderr, "Error: initFile could initialize file with invalid path \"%s\".\n", path);
        
        return invalid_file;
    }
    
    if(!isValidFileName(name, strlen(name)))
    {
        fprintf(stderr, "Error: initFile could initialize file with invalid file name \"%s\".\n", name);
        
        return invalid_file;
    }
    
    char* xtnsn = getFileTypeExtension(type);
    const size_t length = ((path_len + name_len + strlen(xtnsn)) + 1);
    char* f_name = (char*) malloc(length * sizeof(char));
    
    snprintf(f_name, length, "%s/%s%s", path, name, xtnsn);
    
    FILE* f = NULL;
    f = fopen(f_name, "R");
    
    if(f == NULL)
    {
        fprintf(stderr, "Error: initFile could not create a file stream for file \"%s\".\n", f_name);
        
        free(f);
        free(f_name);
        return invalid_file;
    }
    
    file* file_strct = (file*) malloc(sizeof(file));
    
    file_strct->path = f_name;
    file_strct->path_len = length;
    file_strct->file = f;
    file_strct->type = type;
    file_strct->open = false;
    file_strct->length = fseek(file_strct->file, 0, SEEK_END);
    
    fseek(file_strct->file, 0, SEEK_SET);
    fclose(f);
    
    files++;
    
    return file_strct;
}

bool isFileOpen(const file f) {return (f.open);}

char* getFileName(const file f) {return f.path;}

size_t getFileNameLen(const file f) {return f.length;}

ftype getFileType(const file f) {return f.type;}

bool FOpen(file* f, const char* mode) 
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FOpen called on invalid file.\n");
        return false;
    }

    if(isFileOpen(*f))
    {
        fprintf(stderr, "Error: FOpen called on already open file: \"%s\".\n", f->path);
        
        return false;
    }
    
    f->file = freopen(f->path, mode, f->file);
    
    if(f->file == NULL)
    {
        fprintf(stderr, "Error: FOpen has Insufficient permissions to open \"%s\" with filemode \"%s\" or file not found.\n", f->path, mode);
        
        return false;
    }
    
    f->open = true;
    return true;
}

bool FClose(file* f)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FClose called on invalid file.\n");
        return false;
    }
    
    if(!isFileOpen(*f))
    {
        fprintf(stderr, "Error: FClose cannot close already closed file: \"%s\".\n", f->path);
        return false;
    }
    
    fclose(f->file);
    f->open = false;
    
    return true;
}

size_t FGetSize(file* f)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FGetSize called on invalid file.\n");
        return 0;
    }
    
    bool close = false;
    if(!isFileOpen(*f))
    {
        if(FOpen(f, "R")) close = true;
        else
        {
            fprintf(stderr, "Error: FGetSize called on closed file: \"%s\" which failed to open.\n", f->path);
        
            return 0;
        }
    }
    
    size_t size = fseek(f->file, 0, SEEK_END);
    fseek(f->file, 0, SEEK_SET);
    
    if(close) FClose(f);
    
    return size;
}


bool FRead(char* dest, const file* f, const size_t dest_size)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FRead called on invalid file.\n");
        return false;
    }
    
    if(!isFileOpen(*f))
    {
        fprintf(stderr, "Error: FRead called on closed file: \"%s\".\n", f->path);
        return false;
    }
    
    fseek(f->file, 0, SEEK_SET);
    fread(dest, sizeof(char), dest_size, f->file);
    
    return true;
}

bool FWrite(file* f, const char* content, const size_t length)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FWrite called on invalid file.\n");
        return false;
    }
    
    if(!isFileOpen(*f))
    {
        fprintf(stderr, "Error: FWrite called on closed file: \"%s\".\n", f->path);
        return false;
    }
    
    f->length = fwrite(content, length, sizeof(char), f->file);
    fflush(f->file);
    
    return true;
}

void FDestruct(file* f)
{
    if(f == invalid_file)
    {
        fprintf(stderr, "Error: FDestruct called on invalid file.\n");
        return;
    }
    
    if(isFileOpen(*f)) FClose(f);
    
    free(f->path);
    
    file* temp = f;
    f = invalid_file;
    
    free(temp);
    
    files--;
}

#ifdef __cplusplus
}
#endif

#endif

// --------------END_FILE_HEADER------------


#define TEST_SIZE ((size_t) 128)

int main() 
{
    char* str = (char*) malloc(TEST_SIZE * sizeof(char)), *str2 = (char*) malloc(TEST_SIZE * sizeof(char));
    snprintf(str, TEST_SIZE, "-c-a-b-x-3");
    
    int_32 begin = 0, end = 3;
    
    //if(strdel(str2, str, begin, end, strlen(str))) 
    
    //snprintf((str + 3), strlen(str), "LOL");
    
    substring(str2, str, begin, end, TEST_SIZE);
    
    fprintf(stdout, "Str2: \"%s\".\n", str2);
    return 0;
}
