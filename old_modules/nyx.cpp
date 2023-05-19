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

using namespace boost;
using namespace boost::algorithm;

static int l_blank(lua_State *L) {
    return 0;
}

static int l_replace(lua_State *L) {
    string haystack = luaL_checkstring(L, 1);
    string needle = luaL_checkstring(L, 2);
    string replacement = luaL_checkstring(L, 3);
    if(haystack.size() > 0 && needle.size() > 0) {
        replace_all(haystack, needle, replacement);
        lua_pushstring(L, haystack.data());
        return 1;
    }
    lua_pushstring(L, string("").data());
    return 1;
}

static int l_split(lua_State *L) {
    string haystack = luaL_checkstring(L, 1);
    string needle = luaL_checkstring(L, 2);
    int x=1;
    vector<string> sv;
    if(haystack.size() > 0 && needle.size() > 0) {
        vector<string>::iterator it;
        split(sv, haystack, is_any_of(needle), token_compress_on);
        lua_newtable(L);
        for(it=sv.begin(); it != sv.end(); it++) {
            //cout << *it << endl;
            lua_pushinteger(L, x);
            lua_pushstring(L, (*it).data());
            lua_settable(L, -3);
            ++x;
        }
    } else{
        return 0;
    }
    return 1;
}

void luaNyx_register(lua_State *L, const luaL_Reg *l) {
    for (; l->name; l++) {
        lua_pushcfunction(L, l->func);
        lua_setglobal(L, l->name);
    }
}

static const luaL_Reg nyx_symbols[] = {
    {"blank", l_blank},
    {"replace", l_replace},
    {"split", l_split},
    {"rand", l_blank},
    {NULL, NULL}
};

int luaopen_nyx(lua_State *L) {
    luaNyx_register(L, nyx_symbols);
    //luaL_register(L, NULL, nyx_symbols);
    return 0;
}
