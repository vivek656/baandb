// Include guard
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "server_model.h"

int add_reponse_header(api_response* response,char* name , char* value); // Add add_reponse_header function declaration
char* create_response_string(api_response *response); // Add create_response_string function declaration

#endif // HTTP_RESPONSE_H