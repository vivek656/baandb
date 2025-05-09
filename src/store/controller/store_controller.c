
#include <cjson/cJSON.h> // Include cJSON library for JSON parsing
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../base/kv_store.h"

#include <cjson/cJSON.h>
#include "../../global/controller/global_controller.h"
#include "../../server/http_request.h"
#include "../../server/http_response.h"
#include "store_controller.h"

kv_store* store;

api_response
put_handler(api_request* request) {
    cJSON* json = cJSON_Parse(request->body);
    if (!json) {
        api_response response = {.status_code = 400, .body = "Invalid JSON format"};
        return response;
    }

    const cJSON* key = cJSON_GetObjectItemCaseSensitive(json, "key");
    const cJSON* value = cJSON_GetObjectItemCaseSensitive(json, "value");

    if (!cJSON_IsString(value) || (value->valuestring == NULL)) {
        cJSON_Delete(json);
        api_response response = {.status_code = 400, .body = "Invalid value format"};
        return response;
    }

    kvs_object* obj = (kvs_object*)malloc(sizeof(kvs_object));
    char* keyString = key->valuestring;
    obj->value = strdup(value->valuestring);
    obj->key = strdup(keyString);
    char error_message[256];
    if (put(store, keyString, obj, error_message, sizeof(error_message)) == -1) {
        fprintf(stderr, "Error: %s\n", error_message);
        free(obj->value);
        free(obj);
        cJSON_Delete(json);
        api_response response = {.status_code = 400, .body = strdup(error_message)};
        //free(error_message)
        return response;
    }

    cJSON_Delete(json);
    api_response response = {.status_code = 200, .body = "Value stored successfully"};
    add_reponse_header(&response, LOCATION.name, keyString);
    return response;
}

api_response
get_handler(api_request* request) {
    const char* key = find_request_param_value(request, "key");
    if (!key) {
        api_response response = {.status_code = 400, .body = "Key not found"};
        return response;
    }

    kvs_object* obj = get(store, key);
    if (!obj) {
        api_response response = {.status_code = 404, .body = "Key not found"};
        return response;
    }
    cJSON* json = cJSON_CreateObject();

    cJSON_AddStringToObject(json, "key", obj->key);
    cJSON_AddStringToObject(json, "value", obj->value);

    api_response response = {.status_code = 200, .body = cJSON_Print(json)};
    return response;
}

api_response
example_handler(api_request* request) {
    const char* user_agent = find_request_header_value(request, "User-Agent");
    if (user_agent) {
        printf("User-Agent: %s\n", user_agent);
    } else {
        printf("User-Agent header not found\n");
    }

    api_response response = {.status_code = 200, .body = "Header checked"};
    return response;
}

void
init_store_controller() {
    store = get_store();
    add_api_endpoint((endpoint){.method = "PUT", .urlPath = "/kv"}, put_handler);
    add_api_endpoint((endpoint){.method = "GET", .urlPath = "/kv"}, get_handler);
    add_api_endpoint((endpoint){.method = "GET", .urlPath = "/example"}, example_handler);
}
