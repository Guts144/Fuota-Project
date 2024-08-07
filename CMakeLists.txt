cmake_minimum_required(VERSION 3.10)

# Project name
project(MQTTCloudConnector)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Paho MQTT C++ library
find_package(PahoMqttCpp REQUIRED)

# Include directories
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/cloud)
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/common)
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/connectivityStack)

# Source files
set(SOURCES
    main.cpp
    sources/cloud/cloudConnector.cpp
    sources/connectivityStack/connectivityStack.cpp
)

# Check if source files exist
foreach(SOURCE ${SOURCES})
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${SOURCE})
        message(FATAL_ERROR "Cannot find source file: ${SOURCE}")
    endif()
endforeach()

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries explicitly
target_link_libraries(${PROJECT_NAME} 
    ${PahoMqttCpp_LIBRARIES}
    paho-mqtt3as
    paho-mqttpp3
    ssl
    crypto
)
