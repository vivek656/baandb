#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "server.h"

header_name LOCATION = {.name = "Location"};
header_name CONTENT_TYPE = {.name = "Content-Type"};
header_name CONTENT_LENGTH = {.name = "Content-Length"};

Server *create_server(int port     )
{
    Server *server = (Server *)malloc(sizeof(Server));
    server->port = port;
    server->bindings = NULL;
    server->binding_count = 0;
    server->max_headers = DEFAULT_MAX_HEADERS; // Set default max_headers
    return server;
}

void set_max_headers(Server *server, int max_headers)
{
    server->max_headers = max_headers;
}

void bind_endpoint(Server *server, endpoint ep, endpoint_handler handler)
{
    server->bindings = (endpoint_binding *)realloc(server->bindings, sizeof(endpoint_binding) * (server->binding_count + 1));
    server->bindings[server->binding_count].ep.method = strdup(ep.method);
    server->bindings[server->binding_count].ep.urlPath = strdup(ep.urlPath);
    server->bindings[server->binding_count].handler = handler;
    server->binding_count++;
}

int compare_endpoints(endpoint *ep1, endpoint *ep2)
{
    return (strcmp(ep1->method, ep2->method) == 0) && (strcmp(ep1->urlPath, ep2->urlPath) == 0);
}

int handle_request(api_request *request, Server *server, api_response *response)
{
    for (int i = 0; i < server->binding_count; i++)
    {
        if (compare_endpoints(&request->ep, &server->bindings[i].ep))
        {
            *response = server->bindings[i].handler(request);
            return 0;
        }
    }
    response->status_code = 404;
    response->body = "Endpoint Mapping not found";
    response->headers = NULL; // Initialize headers to NULL
    return 0; // Indicate success
}

void free_request(api_request *request)
{
    if (request->headers)
    {
        int header_count = 0;
        for (header_pair *header = request->headers; header->key != NULL; header++)
        {
            free(header->key);
            free(header->value);
            header_count++;
        }
        free(request->headers[header_count].key);
        free(request->headers);
    }

    if (request->params)
    {
        int params_count = 0;
        for (params_pair *param = request->params; param->key != NULL; param++)
        {
            free(param->key);
            free(param->value);
            params_count++;
        }
        free(request->params[params_count].key);
        free(request->params);
    }
}

void free_response(api_response *response)
{
    if (response->headers)
    {
        int header_count = 0;
        for (header_pair *header = response->headers; header->key != NULL; header++)
        {
            free(header->key);
            free(header->value);
            header_count++;
        }
        free(response->headers[header_count].key);
        free(response->headers);
    }
}

void handle_client(int client_socket, Server *server)
{
    char buffer[BUFFER_SIZE];
    int received;
    header_pair *headers = NULL;
    params_pair *params = NULL;
    int header_count = 0;
    int params_count = 0;
    char *body = NULL;
    api_request request = {0}; // Initialize request to zero
    api_response response = {0}; // Initialize response

    received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (received < 0)
    {
        perror("recv failed");
        dprintf(client_socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %zu\r\n\r\n%s", strlen("recv failed"), "recv failed");
        goto cleanup;
    }
    buffer[received] = '\0';
    // Print the buffer
    printf("Received buffer:\n%s\n", buffer);
    char method[8], url[256], protocol[16];
    sscanf(buffer, "%s %s %s", method, url, protocol);

    // Parse headers
    headers = (header_pair *)malloc(sizeof(header_pair) * server->max_headers);
    if (!headers)
    {
        perror("malloc failed");
        dprintf(client_socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %zu\r\n\r\n%s", strlen("malloc failed"), "malloc failed");
        goto cleanup;
    }
    char *header_line = strstr(buffer, "\r\n") + 2;
    while (header_line && *header_line != '\r' && header_count < server->max_headers)
    {
        char *next_line = strstr(header_line, "\r\n");
        if (!next_line)
            break;
        *next_line = '\0';

        char *colon = strstr(header_line, ": ");
        if (colon)
        {
            *colon = '\0';
            headers[header_count].key = strdup(header_line);
            headers[header_count].value = strdup(colon + 2);
            if (!headers[header_count].key || !headers[header_count].value)
            {
                perror("strdup failed");
                dprintf(client_socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %zu\r\n\r\n%s", strlen("strdup failed"), "strdup failed");
                goto cleanup;
            }
            header_count++;
        }
        header_line = next_line + 2;
    }
    headers[header_count].key = NULL; // Null-terminate the headers array

    // Parse body
    body = strstr(header_line, "\r\n");
    printf("Received body:\n%s\n", body);
    if (body)
    {
        body += 2;
    }
    else
    {
        body = "";
    }

    // Parse params from URL
    char *query_string = strchr(url, '?');
    if (query_string)
    {
        *query_string = '\0'; // Terminate URL at '?'
        query_string++;
        params = (params_pair *)malloc(sizeof(params_pair) * 10); // Allocate space for params
        if (!params)
        {
            perror("malloc failed");
            dprintf(client_socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %zu\r\n\r\n%s", strlen("malloc failed"), "malloc failed");
            goto cleanup;
        }
        char *param = strtok(query_string, "&");
        while (param && params_count < 10)
        {
            char *equal_sign = strchr(param, '=');
            if (equal_sign)
            {
                *equal_sign = '\0';
                params[params_count].key = strdup(param);
                params[params_count].value = strdup(equal_sign + 1);
                if (!params[params_count].key || !params[params_count].value)
                {
                    perror("strdup failed");
                    dprintf(client_socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %zu\r\n\r\n%s", strlen("strdup failed"), "strdup failed");
                    goto cleanup;
                }
                params_count++;
            }
            param = strtok(NULL, "&");
        }
        params[params_count].key = NULL; // Null-terminate the params array
    }

    endpoint ep = {
        .method = method,
        .urlPath = url};
    request = (api_request){
        .ep = ep,
        .headers = headers,
        .params = params, // Add params to request
        .body = body};

    if (handle_request(&request, server, &response) < 0)
    {
        dprintf(client_socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %zu\r\n\r\n%s", strlen(response.body), response.body);
    }
    else
    {
        char *response_string = create_response_string(&response);
        if (response_string)
        {
            send(client_socket, response_string, strlen(response_string), 0);
           // free(response_string);
        }
    }
    goto cleanup;
cleanup:
    free_request(&request);
    free_response(&response); // Use the new free_response method
   
}

int start_server(Server *server)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(server->port)};

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 10) < 0)
    {
        perror("listen");
        close(server_socket);
        return 1;
    }

    printf("Server running on port %d\n", server->port);

    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }
        handle_client(client_socket, server);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}

void free_server(Server *server)
{
    for (int i = 0; i < server->binding_count; i++)
    {
        free(server->bindings[i].ep.method);
        free(server->bindings[i].ep.urlPath);
    }
    free(server->bindings);
    free(server);
}