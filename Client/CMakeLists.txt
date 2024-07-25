cmake_minimum_required(VERSION 3.24.)
project(client)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

file(GLOB project
    src/*
)

add_executable(client WIN32 ${project})
target_link_libraries(client ${wxWidgets_LIBRARIES})