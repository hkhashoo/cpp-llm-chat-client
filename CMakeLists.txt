cmake_minimum_required(VERSION 3.20)
project(cpp_llm_chat_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(llmchat
    src/main.cpp
)
target_include_directories(llmchat PRIVATE include)

add_custom_target(run
    COMMAND llmchat
    DEPENDS llmchat
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)