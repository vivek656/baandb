#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "kv_store.h"


kv_store* __global_kv_store = NULL;

// Create a key-value store
kv_store* create_store() {
    kv_store* store = (kv_store*)malloc(sizeof(kv_store));
    store->table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    return store;
}

kv_store* store_reference() {
    return __global_kv_store;
}

kv_store* get_store() {
    if(__global_kv_store == NULL) {
        __global_kv_store = create_store();
    }
    return __global_kv_store;
}

// Free the memory allocated for the key-value store
void free_store(kv_store* store) {
    g_hash_table_destroy(store->table);
    free(store);
}

int is_valid_key(const char* key, char* error , int error_msg_size) {
    int length = strlen(key);
    if (length == 0 || length > 256) {
        if (error) {
            snprintf(error, error_msg_size, "Key length must be between 1 and 256 characters");
        }
        return 0;
    }
    if (!g_ascii_isalnum(key[0])) {
        if (error) {
            snprintf(error, error_msg_size, "Key must start with an alphanumeric character");
        }
        return 0;
    }
    return 1;
}

// Add or update a key-value pair in the store
int put(kv_store* store, const char* key, kvs_object* value,char *error, int error_msg_size) {
    if (!is_valid_key(key, error,error_msg_size)) {
        fprintf(stderr, "Invalid key format\n");
        return -1;
    }
    g_hash_table_insert(store->table, strdup(key), value);
    return 0;
}

// Retrieve the value associated with a key
kvs_object* get(kv_store* store, const char* key) {
    return (kvs_object*)g_hash_table_lookup(store->table, key);
}

// Remove a key-value pair from the store
int delete_entry(kv_store* store, const char* key) {
    return g_hash_table_remove(store->table, key) ? 0 : -1;
}