cmake_minimum_required(VERSION 3.20)

project(orangepilite_gpio)

# Command to create symlink for clangd to see all includes
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(SOURCES 
	include/gpio_socket.h
	src/gpio_socket.cpp
	include/gpio_pin.h
	src/gpio_pin.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PUBLIC include/ )
