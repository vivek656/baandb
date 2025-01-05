#ifndef SERVER_MODEL_H
#define SERVER_MODEL_H

#define BUFFER_SIZE 1024
#define DEFAULT_MAX_HEADERS 50

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

typedef api_response (*endpoint_handler)(api_request* request);

typedef struct {
    endpoint ep; // Use the new endpoint struct
    endpoint_handler handler;
} endpoint_binding;

typedef struct {
    int port;
    endpoint_binding* bindings;
    int binding_count;
    int max_headers; // Add max_headers to Server struct
} Server;

#endif // SERVER_H

