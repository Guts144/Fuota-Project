cmake_minimum_required(VERSION 3.10)

# Project name
project(MQTTCloudConnector)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Paho MQTT C++ library
find_package(PahoMqttCpp REQUIRED)

# Find the CURL library
find_package(CURL REQUIRED)

# Add the include directories for CURL
include_directories(${CURL_INCLUDE_DIRS})

# Include directories
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/cloud)
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/common)
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/connectivityStack)
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/raucManager)
include_directories(${PahoMqttCpp_INCLUDE_DIRS} sources/downloader)

# Source files
set(SOURCES
    main.cpp
    sources/cloud/cloudConnector.cpp
    sources/connectivityStack/connectivityStack.cpp
    sources/downloader/downloader.cpp
)

# Check if source files exist
foreach(SOURCE ${SOURCES})
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${SOURCE})
        message(FATAL_ERROR "Cannot find source file: ${SOURCE}")
    endif()
endforeach()

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the CURL library
target_link_libraries(MQTTCloudConnector ${CURL_LIBRARIES})
# Link libraries explicitly
target_link_libraries(${PROJECT_NAME} 
    ${PahoMqttCpp_LIBRARIES}
    paho-mqtt3as
    paho-mqttpp3
    ssl
    crypto
)
# clang-format
#
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    message(STATUS "Will use clang-format tool located at ${CLANG_FORMAT}")
    # Step 1 list the files to be formated.
    file(GLOB_RECURSE SOURCE_FILES_TO_FORMAT RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.hpp)
    foreach (SOURCE_FILE ${SOURCE_FILES_TO_FORMAT})
        string(FIND ${SOURCE_FILE} "sources/" PROJECT_SOURCES_DIR_FOUND)
        if (NOT ${PROJECT_SOURCES_DIR_FOUND} EQUAL 0 AND NOT ${PROJECT_TESTS_DIR_FOUND} EQUAL 0)
            list(REMOVE_ITEM SOURCE_FILES_TO_FORMAT ${SOURCE_FILE})
        endif ()
    endforeach ()
    list(TRANSFORM SOURCE_FILES_TO_FORMAT PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
    message(NOTICE "Files to be clang-format checked : ${SOURCE_FILES_TO_FORMAT}")

    # Step 2 check the formating during build.
    add_custom_target(
        clangformat-check
        ALL
        COMMAND ${CLANG_FORMAT}
        -style=file
        --dry-run
        --Werror
        ${SOURCE_FILES_TO_FORMAT}
    )

    # Step 3 Manually run "cmake clangformat-correct" to fix the format of files.
    # Warning ! This will edit the source files inplace, commit your other changes before !
    add_custom_target(
        clangformat-correct
        COMMAND ${CLANG_FORMAT}
        -style=file
        -i
        ${SOURCE_FILES_TO_FORMAT}
    )
else()
    message(STATUS "clang-format not found")
endif()

