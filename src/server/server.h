#ifndef SERVER_H
#define SERVER_H

#include "server_model.h"

Server* create_server(int port);
void set_max_headers(Server* server, int max_headers); // Add set_max_headers function declaration
void bind_endpoint(Server* server, endpoint ep, endpoint_handler handler); // Update bind_endpoint function declaration
int start_server(Server* server);
void free_server(Server* server);
void free_response(api_response *response); // Add free_response function declaration
int add_reponse_header(api_response* response,char* name , char* value); // Add add_reponse_header function declaration
char* create_response_string(api_response *response); // Add create_response_string function declaration


#endif // SERVER_H           