set(SOURCES
    Api.h Api.cpp
    ApiServer.h ApiServer.cpp
)

include_directories(BEFORE ../ethminer/ ../beast/include/)
add_library(apicore ${SOURCES})
target_link_libraries(apicore PRIVATE devcore ethminer-buildinfo libjson-rpc-cpp::server)
target_include_directories(apicore PRIVATE ..)
