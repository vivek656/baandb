#include "http_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to concatenate two strings
char* concat_strings(const char* str1, const char* str2) {
    // Calculate the total length needed
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t total_length = len1 + len2;

    // Allocate memory for the concatenated string
    char* result = (char*)malloc(total_length + 1); // +1 for the null terminator
    if (!result) {
        perror("malloc failed");
        return NULL;
    }

    // Copy the first string
    strcpy(result, str1);

    // Concatenate the second string
    strcat(result, str2);

    return result;
}

int add_reponse_header(api_response* response,char* name , char* value){
    int header_count = 0;
    if(response->headers){
        for(header_pair* header = response->headers; header->key != NULL; header++){
            header_count++;
        }
    }
    response->headers = (header_pair*)realloc(response->headers, sizeof(header_pair) * (header_count + 2));
    response->headers[header_count].key = strdup(name);
    response->headers[header_count].value = strdup(value);
    response->headers[header_count + 1].key = NULL;
    return 0;
}

char* create_response_string(api_response *response)
{
    // Start with the status line
    char *response_string = concat_strings("HTTP/1.1 ", "");
    char status_code_str[4];
    sprintf(status_code_str, "%d", response->status_code);
    response_string = concat_strings(response_string, status_code_str);
    response_string = concat_strings(response_string, "\r\n");

    // Add headers
    if (response->headers)
    {
        for (header_pair *header = response->headers; header->key != NULL; header++)
        {
            response_string = concat_strings(response_string, header->key);
            response_string = concat_strings(response_string, ": ");
            response_string = concat_strings(response_string, header->value);
            response_string = concat_strings(response_string, "\r\n");
        }
    }

    // Add Content-Length header
    char content_length_str[40];
    sprintf(content_length_str, "Content-Length: %zu\r\n\r\n", strlen(response->body));
    response_string = concat_strings(response_string, content_length_str);

    // Add body
    response_string = concat_strings(response_string, response->body);

    return response_string;
}
