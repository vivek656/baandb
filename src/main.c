#include <stdio.h>
#include <uuid/uuid.h>
#include "global/controller/global_controller.h"
#include "store/base/kv_store.h"
#include <cjson/cJSON.h> // Include cJSON library for JSON parsing
#include "server/server_model.h"
#include "store/controller/store_controller.h"

#include "server/server.h"

void free_all(
    Server* server,
    kv_store* store
) {
    free_server(server);
    free_store(store);
}


int main() {
    // Initialize the key-value store
    kv_store* store = get_store();
    // Create the server on port 8888
    Server* server = create_server(8888);
    // Set the global controller with the server
    set_global_controller(server);


    //initialize dependencies
    init_store_controller();

    // Start the server
    if (start_server(server) != 0) {
        fprintf(stderr, "Failed to start server\n");
        free_all(server, store);
        return 1;
    }

    // Free resources
    free_all(server, store);
    return 0;
}

