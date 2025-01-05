#include <stdlib.h>
#include "global_controller.h"
#include "../../server/server.h"

Global_controller* __global_controller = NULL;

Global_controller* set_global_controller(Server* server) {
    if (__global_controller == NULL) {
        __global_controller = (Global_controller*)malloc(sizeof(Global_controller));
    }
    __global_controller->server = server;
    return __global_controller;
}

void add_api_endpoint(endpoint ep, endpoint_handler handler) {
    bind_endpoint(__global_controller->server, ep, handler);
}