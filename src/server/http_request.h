#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "server_model.h"

char* find_request_param_value(api_request* request, const char* param_name);
char* find_request_header_value(api_request* request, const char* header_name);

#endif // HTTP_REQUEST_H