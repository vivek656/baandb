#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>
#include "kv_store.h"
#include "server.h"
#include <cjson/cJSON.h> // Include cJSON library for JSON parsing


api_response put_handler(api_request* request, kv_store* store) {
    cJSON *json = cJSON_Parse(request->body);
    if (!json) {
        api_response response = { .status_code = 400, .body = "Invalid JSON format" };
        return response;
    }

    const cJSON *key = cJSON_GetObjectItemCaseSensitive(json, "key");
    const cJSON *value = cJSON_GetObjectItemCaseSensitive(json, "value");

    if (!cJSON_IsString(value) || (value->valuestring == NULL)) {
        cJSON_Delete(json);
        api_response response = { .status_code = 400, .body = "Invalid value format" };
        return response;
    }

    char uuid_str[37];
    if (cJSON_IsString(key) && (key->valuestring != NULL)) {
        strncpy(uuid_str, key->valuestring, sizeof(uuid_str));
    } else {
        uuid_t uuid;
        uuid_generate(uuid);
        uuid_unparse(uuid, uuid_str);
    }

    kvs_object* obj = (kvs_object*)malloc(sizeof(kvs_object));
    obj->value = strdup(value->valuestring);

    if (put(store, uuid_str, obj) == -1) {
        free(obj->value);
        free(obj);
        cJSON_Delete(json);
        api_response response = { .status_code = 400, .body = "Failed to store value" };
        return response;
    }

    cJSON_Delete(json);
    api_response response = { .status_code = 200, .body = "Value stored successfully" };
    add_reponse_header(&response, LOCATION.name , uuid_str);
    return response;
}

api_response get_handler(api_request* request, kv_store* store) {
    const char *key = find_param_value(request, "key");
    if (!key) {
        api_response response = { .status_code = 400, .body = "Key not found" };
        return response;
    }

    kvs_object* obj = get(store, key);
    if (!obj) {
        api_response response = { .status_code = 404, .body = "Key not found" };
        return response;
    }
    api_response response = { .status_code = 200, .body = (char*)obj->value };
    return response;
}

api_response example_handler(api_request* request, kv_store* store) {
    const char* user_agent = find_header_value(request, "User-Agent");
    if (user_agent) {
        printf("User-Agent: %s\n", user_agent);
    } else {
        printf("User-Agent header not found\n");
    }

    api_response response = { .status_code = 200, .body = "Header checked" };
    return response;
}

int main() {
    // Initialize the key-value store
    kv_store* store = create_store();

    // Create the server on port 8888
    Server* server = create_server(8888, store);

    // Bind endpoints to the server
    bind_endpoint(server, (endpoint){.method = "PUT", .urlPath = "/kv"}, put_handler);
    bind_endpoint(server, (endpoint){.method = "GET", .urlPath = "/kv"}, get_handler);
    bind_endpoint(server, (endpoint){.method = "GET", .urlPath = "/example"}, example_handler);

    // Start the server
    if (start_server(server) != 0) {
        fprintf(stderr, "Failed to start server\n");
        free_server(server);
        free_store(store);
        return 1;
    }

    // Free resources
    free_server(server);
    free_store(store);
    return 0;
}
