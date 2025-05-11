cmake_minimum_required(VERSION 3.15)
project(otlp_collector VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies

find_package(gRPC REQUIRED)
find_package(YAML-CPP REQUIRED)
find_package(opentelemetry-cpp CONFIG REQUIRED)
find_package(Protobuf REQUIRED)
find_package(yaml-cpp REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/collector.cpp
    src/processor.cpp
    src/receiver.cpp
    src/exporter.cpp
)

# Executable
add_executable(otlp_collector ${SOURCES})

target_include_directories(otlp_collector PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OPENTELEMETRY_CPP_INCLUDE_DIRS}
    src
)

target_link_libraries(otlp_collector PRIVATE
    ${OPENTELEMETRY_CPP_LIBRARIES}
    gRPC::grpc++
    protobuf::libprotobuf
    yaml-cpp::yaml-cpp
)

# Copy config file to build directory
configure_file(config/config.yaml ${CMAKE_CURRENT_BINARY_DIR}/config.yaml COPYONLY)

# Install
install(TARGETS otlp_collector DESTINATION bin)
install(FILES config/config.yaml DESTINATION etc/otlp_collector)
