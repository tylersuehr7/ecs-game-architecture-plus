cmake_minimum_required(VERSION 3.20)
project(ecs_plus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    SOURCES
    src/main.cpp
)

set(
    EXAMPLE_SOURCES
    src/demo/main.cpp
    src/demo/components.hpp
    src/demo/systems.hpp
    src/ecs/component_array.hpp
    src/ecs/component_manager.hpp
    src/ecs/entity_manager.hpp
    src/ecs/entity.hpp
    src/ecs/system_manager.hpp
    src/ecs/system.hpp
    src/ecs/world.hpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

add_executable(
    ecs_example
    ${EXAMPLE_SOURCES}
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_include_directories(
    ecs_example
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src
)
