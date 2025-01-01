#ifndef SERVER_H
#define SERVER_H

#include "kv_store.h"

typedef struct {
    int status_code;
    char* body;
} api_response;

// New endpoint struct
typedef struct {
    char* method;
    char* urlPath;
} endpoint;

typedef struct {
    char* key;
    char* value;
} header_pair;

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

#endif // SERVER_H

