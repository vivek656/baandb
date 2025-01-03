#ifndef KV_STORE_H
#define KV_STORE_H

#include <glib.h>

// Define a structure for the kvs_object
typedef struct {
    char* uuid; // UUID key
    void* value; // Value that can store any type
} kvs_object;

// Define a structure for the key-value store
typedef struct {
    GHashTable* table; // Hash table for key-value pairs
} kv_store;

// Function declarations
kv_store* create_store();
void free_store(kv_store* store);
int put(kv_store* store, const char* uuid, kvs_object* value);
kvs_object* get(kv_store* store, const char* uuid);
int delete_entry(kv_store* store, const char* uuid);

#endif // KV_STORE_H