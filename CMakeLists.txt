cmake_minimum_required(VERSION 3.25)
project(MCP_Spting2023)

set(CMAKE_CXX_STANDARD 17)

add_executable(MCP_Spting2023 main.cpp main.cpp TLS_OMP.cpp TLS_Serial.cpp TLS_Task.cpp)
