cmake_minimum_required(VERSION 3.5)

project(sprite-engine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(dependencies/raylib)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# lib
add_library(sprite-engine-lib
    src/sz_sprite.c
    src/sz_character.c
    src/sz_app.c
)
target_link_libraries(sprite-engine-lib raylib)

# test sprites
add_executable(sprite-test
    src/sprite_test.c
)

target_link_libraries(sprite-test sprite-engine-lib)

# test character
add_executable(character-test
    src/character_test.c
)

target_link_libraries(character-test sprite-engine-lib)

# helper for generating sprite sheets
add_executable(generate-sprite-sheet
    src/sz_sprite.c
    src/sprite_sheet_generator/generate_sprite_sheet.c
)

target_link_libraries(generate-sprite-sheet raylib)


