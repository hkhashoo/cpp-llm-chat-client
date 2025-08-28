cmake_minimum_required(VERSION 3.20)
project(cpp_llm_chat_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(llmchat
    src/main.cpp
    src/http_client.cpp
    src/chat.cpp
    src/util_env.cpp
    src/util_log.cpp
)
target_include_directories(llmchat PRIVATE include)

add_custom_target(run
    COMMAND llmchat
    DEPENDS llmchat
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# --- third-party deps (header-only json + HTTP client) ---
include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.5
)
set(CPR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0
)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

target_link_libraries(llmchat PRIVATE cpr::cpr nlohmann_json::nlohmann_json spdlog::spdlog)
