#ifndef SERVER_H
#define SERVER_H

#include "kv_store.h"
#pragma once

typedef struct {
    char* name;
} header_name;

extern header_name LOCATION;
extern header_name CONTENT_TYPE;
extern header_name CONTENT_LENGTH;

typedef struct {
    char* key;
    char* value;
} header_pair;


typedef struct {
    int status_code;
    char* body;
    header_pair* headers; // Add headers to api_response
} api_response;

// New endpoint struct
typedef struct {
    char* method;
    char* urlPath;
} endpoint;

// Add params_pair struct
typedef struct {
    char* key;
    char* value;
} params_pair;

typedef struct {
    endpoint ep;
    header_pair* headers;
    params_pair* params; // Add params to api_request
    char* body;
} api_request;

typedef api_response (*endpoint_handler)(api_request* request, kv_store* store);

typedef struct {
    endpoint ep; // Use the new endpoint struct
    endpoint_handler handler;
} endpoint_binding;

typedef struct {
    int port;
    kv_store* store;
    endpoint_binding* bindings;
    int binding_count;
    int max_headers; // Add max_headers to Server struct
} Server;



Server* create_server(int port, kv_store* store);
void set_max_headers(Server* server, int max_headers); // Add set_max_headers function declaration
void bind_endpoint(Server* server, endpoint ep, endpoint_handler handler); // Update bind_endpoint function declaration
int start_server(Server* server);
void free_server(Server* server);
const char *find_param_value(api_request *request, const char *param_name);
const char *find_header_value(api_request *request, const char *header_name);
void free_response(api_response *response); // Add free_response function declaration
int add_reponse_header(api_response* response,char* name , char* value); // Add add_reponse_header function declaration
char* create_response_string(api_response *response); // Add create_response_string function declaration


#endif // SERVER_H