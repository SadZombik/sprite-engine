cmake_minimum_required(VERSION 3.5)

project(sprite-engine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)


if (WIN32)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Dependencies
add_subdirectory(dependencies/raylib)
add_subdirectory(dependencies/lua)

# lib
add_library(sprite-engine-lib
    src/sz_sprite.c
    src/sz_character.c
    src/sz_app.c
)
target_link_libraries(sprite-engine-lib PUBLIC raylib)
target_link_libraries(sprite-engine-lib PUBLIC lua_library)

# tests
set(tests_targets
	sprite_test
    character_test
)

foreach(target ${tests_targets})
	file(GLOB SOURCES tests/${target}.c)
	foreach(SOURCE ${SOURCES})
		get_filename_component(FILENAME ${SOURCE} NAME_WE)
        message(STATUS "Adding test target: ${FILENAME}")
		add_executable(${FILENAME} ${SOURCE})

		target_include_directories(${FILENAME} PRIVATE src/)
		target_link_libraries(${FILENAME} sprite-engine-lib)
	endforeach()
endforeach()

# lua test
add_executable(lua_test
	tests/lua_test.c
)

target_include_directories(lua_test PRIVATE dependencies/lua/src)
target_link_libraries(lua_test lua_library)

# helper for generating sprite sheets
add_executable(generate-sprite-sheet
    src/sz_sprite.c
    src/sprite_sheet_generator/generate_sprite_sheet.c
)

target_link_libraries(generate-sprite-sheet raylib)
