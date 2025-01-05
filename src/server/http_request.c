#include "http_request.h"
#include <stdio.h>
#include <string.h>

char* find_request_param_value(api_request* request, const char* param_name) {
    for (params_pair* param = request->params; param->key != NULL; param++) {
        if (strcmp(param->key, param_name) == 0) {
            return param->value;
        }
    }
    return NULL;
}

char* find_request_header_value(api_request* request, const char* header_name) {
    for (header_pair* header = request->headers; header->key != NULL; header++) {
        if (strcmp(header->key, header_name) == 0) {
            return header->value;
        }
    }
    return NULL;
}