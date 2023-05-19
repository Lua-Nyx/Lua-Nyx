/*
* Copyright (C) 2011, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "main.hpp"
#include "sqlite3.h"

using namespace boost;
using namespace boost::algorithm;

typedef struct {
    lua_State *L;
    int counter;
} lsqlite3_callback_s;

static int l_blank(lua_State *L) {
    return 0;
}

static int lsqlite3_callback(void *ud, int argc, char **argv, char **colname) {
    lsqlite3_callback_s *cb = (lsqlite3_callback_s *)ud;
    lua_State *L=cb->L;
    int x;
    if(argc) // make sure there is arguments before making table
        lua_newtable(L);
    for(x=0; x<argc; x++) {
        lua_pushstring(L, colname[x]);
        lua_pushstring(L, argv[x]);
        lua_settable(L, -3);
    }
    lua_rawseti(L, -2, cb->counter);
    cb->counter++;
    return 0;
}

static int l_sqlite3_new(lua_State *L) {
    sqlite3 **s = (sqlite3 **)lua_newuserdata(L, sizeof(sqlite3 *));
    luaL_getmetatable(L, "nyx.sqlite3");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    *s = NULL;
    return 1;
}

static int lsqlite3_open(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    string path(luaL_checkstring(L, 2));
    if(*s == NULL) {
        int ret;
        if(ret=sqlite3_open(path.data(), &(*s))) {
            lua_pushboolean(L, 0);
            //luaL_error(L, "sqlite3:open: can't open dbase %i", ret);
            return 1;
        }
    } else {
        lua_pushboolean(L, 0);
        //luaL_error(L, "sqlite3:open: database is already open");
        return 1;
    }
    lua_pushboolean(L, 1);
    return 1;
}

static int lsqlite3_close(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    int ret;
    if(s) {
        if((ret=sqlite3_close(*s)) != SQLITE_OK) {
            //luaL_error(L, "sqlite3:__gc: Error collecting sqlite3 %i", ret);
            lua_pushboolean(L, 0);
            return 1;
        }
    }
    lua_pushboolean(L, 1);
    return 1;
}

static int lsqlite3_exec(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    string stmt = luaL_checkstring(L, 2);
    if(s) {
        char *error_msg;
        int ret;
        if(stmt.size() > 0) { // having an actual sql statement to run is nice
            lua_newtable(L); // start of records table
            lsqlite3_callback_s cb;
            cb.L = L; cb.counter = 1;
            if((ret=sqlite3_exec((*s), stmt.data(), lsqlite3_callback, (void *)&cb, &error_msg)) != SQLITE_OK) {
                luaL_error(L, "sqlite3:exec: %i : %s", ret, error_msg);
            }
        }
    } else {
        luaL_error(L, "sqlite3:exec: SQLite3 DB is not opened");
    }
    return 1;
}

static int lsqlite3_version(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    lua_pushstring(L, SQLITE_VERSION);
    return 1;
}

static int lsqlite3_escape(lua_State *L) { // Basic Escape Function
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    string r = luaL_checkstring(L, 2);
    replace_all(r, "'", "\\'");
    replace_all(r, "\"", "\\\"");
    replace_all(r, "\\", "\\\\");
    lua_pushstring(L, r.data());
    return 1;
}

static int lsqlite3_unescape(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    string r = luaL_checkstring(L, 2);
    replace_all(r, "\\'", "'");
    replace_all(r, "\\\"", "\"");
    replace_all(r, "\\\\", "\\");
    lua_pushstring(L, r.data());
    return 1;
}

static int lsqlite3_changes(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    lua_pushinteger(L, sqlite3_changes(*s));
    return 1;
}

static int lsqlite3_total_changes(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    lua_pushinteger(L, sqlite3_changes(*s));
    return 1;
}

static int lsqlite3_gc(lua_State *L) {
    sqlite3 **s = (sqlite3 **)luaL_checkudata(L, 1, "nyx.sqlite3");
    int ret;
    if(*s) {
        if((ret=sqlite3_close((*s))) != SQLITE_OK) {
            luaL_error(L, "sqlite3:__gc: Error collecting sqlite3 %i", ret);
        }
    }
    return 0;
}

static const luaL_Reg sqlite3f[] = {
    {"new", l_sqlite3_new},
    {NULL, NULL}
};

static const luaL_Reg sqlite3m[] = {
    {"open", lsqlite3_open},
    {"close", lsqlite3_close},
    {"exec", lsqlite3_exec},
    {"version", lsqlite3_version},
    {"changes", lsqlite3_changes},
    {"total_changes", lsqlite3_total_changes},
    {"escape", lsqlite3_escape},
    {"unescape", lsqlite3_unescape},
    {"__gc", lsqlite3_gc},
    {NULL, NULL}
};

int luaopen_sqlite3(lua_State *L) {
    luaL_newmetatable(L, "nyx.sqlite3");
    luaL_register(L, NULL, sqlite3m);
    luaL_register(L, "sqlite3", sqlite3f);
    return 1;
}
