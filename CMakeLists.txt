cmake_minimum_required(VERSION 3.10)
project(baandb VERSION 0.1.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include pkg-config
include(FindPkgConfig)

# Find dependencies
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(APR REQUIRED apr-1)
pkg_check_modules(APRUTIL REQUIRED apr-util-1)
pkg_check_modules(cJSON REQUIRED libcjson)

# Add the executable and sources
add_executable(baandb
    src/main.c
    src/store/base/kv_store.c
    src/server/server.c
    src/server/http_request.c
    src/server/http_response.c
    src/global/controller/global_controller.c
    src/store/controller/store_controller.c
    src/thread/threadpool.c
)

# Include directories
target_include_directories(baandb PRIVATE
    ${GLIB_INCLUDE_DIRS}
    ${APR_INCLUDE_DIRS}
    ${APRUTIL_INCLUDE_DIRS}
    ${cJSON_INCLUDE_DIRS}
)

# Compile definitions/flags
target_compile_definitions(baandb PRIVATE
    ${GLIB_CFLAGS_OTHER}
    ${APR_CFLAGS_OTHER}
    ${APRUTIL_CFLAGS_OTHER}
    ${cJSON_CFLAGS_OTHER}
)

# Link libraries
target_link_libraries(baandb PRIVATE
    ${GLIB_LIBRARIES}
    ${APR_LIBRARIES}
    ${APRUTIL_LIBRARIES}
    ${cJSON_LIBRARIES}
)

# Optional packaging metadata
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
