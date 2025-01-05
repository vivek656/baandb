cmake_minimum_required(VERSION 3.10)
project(baandb VERSION 0.1.0 LANGUAGES C)

# Include the FindPkgConfig module
include(FindPkgConfig)

# Find GLib package
pkg_check_modules(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
add_definitions(${GLIB_CFLAGS_OTHER})

# Find UUID package
pkg_check_modules(UUID REQUIRED uuid)
include_directories(${UUID_INCLUDE_DIRS})
link_directories(${UUID_LIBRARY_DIRS})
add_definitions(${UUID_CFLAGS_OTHER})

# Find and include cJSON
find_package(cJSON REQUIRED)
include_directories(${cJSON_INCLUDE_DIRS})

# Add executable
add_executable(
    baandb src/main.c 
    src/kv_store.c src/server/server.c src/server/http_request.c)

# Link against GLib, UUID, and cJSON
target_link_libraries(baandb ${GLIB_LIBRARIES} ${UUID_LIBRARIES} cjson)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

