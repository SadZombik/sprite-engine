#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>

static int lua_test_function(lua_State *L) {
    lua_pushstring(L, "Hello from C!");
    return 1; // Number of return values
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <script.lua>\n", argv[0]);
        return 1;
    }

    lua_State *L = luaL_newshowtate(); // Create a new Lua state
    luaL_openlibs(L);               // Load standard libraries

    lua_register(L, "test_function", lua_test_function);

    if (luaL_dofile(L, argv[1]) != LUA_OK) {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1); // Remove error message from stack
    }

    lua_close(L);  
    return 0;
}
