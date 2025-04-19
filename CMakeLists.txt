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


# Find APR package
pkg_check_modules(APR REQUIRED apr-1)
include_directories(${APR_INCLUDE_DIRS})
link_directories(${APR_LIBRARY_DIRS})
add_definitions(${APR_CFLAGS_OTHER})

pkg_check_modules(APRUTIL REQUIRED apr-util-1)
include_directories(${APRUTIL_INCLUDE_DIRS})
link_directories(${APRUTIL_LIBRARY_DIRS})
add_definitions(${APRUTIL_CFLAGS_OTHER})


# Find and include cJSON
find_package(cJSON REQUIRED)
include_directories(${cJSON_INCLUDE_DIRS})

# Add executable
add_executable(
    baandb src/main.c 
    src/store/base/kv_store.c src/server/server.c src/server/http_request.c src/server/http_response.c
    src/global/controller/global_controller.c src/store/controller/store_controller.c
    src/thread/threadpool.c
    )

# Link against GLib, UUID, and cJSON
target_link_libraries(baandb ${GLIB_LIBRARIES} ${UUID_LIBRARIES} ${APR_LIBRARIES}  ${APRUTIL_LIBRARIES} cjson)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

