#ifndef GLOBAL_CONTROLLER_H
#define GLOBAL_CONTROLLER_H

#include "../../server/server_model.h"

typedef struct {
    Server* server;
} Global_controller;

extern Global_controller* __global_controller;

Global_controller* set_global_controller(Server* server);

void add_api_endpoint(endpoint ep, endpoint_handler handler);

#endif