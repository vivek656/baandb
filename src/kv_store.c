#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <uuid/uuid.h>
#include "kv_store.h"

// Create a key-value store
kv_store* create_store() {
    kv_store* store = (kv_store*)malloc(sizeof(kv_store));
    store->table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    return store;
}

// Free the memory allocated for the key-value store
void free_store(kv_store* store) {
    g_hash_table_destroy(store->table);
    free(store);
}

// Function to validate UUID format using libuuid
int is_valid_uuid(const char* uuid) {
    uuid_t binuuid;
    return uuid_parse(uuid, binuuid) == 0;
}

// Add or update a key-value pair in the store
int put(kv_store* store, const char* uuid, kvs_object* value) {
    if (!is_valid_uuid(uuid)) {
        fprintf(stderr, "Invalid UUID format\n");
        return -1;
    }
    g_hash_table_insert(store->table, strdup(uuid), value);
    return 0;
}

// Retrieve the value associated with a key
kvs_object* get(kv_store* store, const char* uuid) {
    return (kvs_object*)g_hash_table_lookup(store->table, uuid);
}

// Remove a key-value pair from the store
int delete_entry(kv_store* store, const char* uuid) {
    return g_hash_table_remove(store->table, uuid) ? 0 : -1;
}