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
using namespace xpressive;
using namespace std;

static int l_blank(lua_State *L) {
    return 0;
}

static int l_replace(lua_State *L) {
    string haystack = luaL_checkstring(L, 1);
    string needle = luaL_checkstring(L, 2);
    string replace = luaL_checkstring(L, 3);
    if(needle.size() > 0 && haystack.size() > 0 && replace.size() > 0) {
        sregex re = as_xpr(needle);
        string result = regex_replace(haystack, re, replace);
        lua_pushstring(L, result.data());
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int l_gmatch(lua_State *L) {
    string haystack = luaL_checkstring(L, 1);
    string needle = luaL_checkstring(L, 2);
    bool got_result = false;
    if(needle.size() > 0 && haystack.size() > 0) {
        boost::regex re(needle);
        boost::smatch match;
        int x=1, y=1;
        lua_newtable(L);
        while(boost::regex_search(haystack, match, re, boost::match_not_dot_newline)) {
            lua_newtable(L);
            for(y=1; y != match.size(); y++) {
                lua_pushnumber(L, y);
                lua_pushstring(L, string(match[y]).data());
                lua_settable(L, -3);
            }
            lua_rawseti(L, -2, x);
            haystack = match.suffix();
            ++x;
            if(!got_result)
                got_result=true;
        }
    }
    if(!got_result)
        lua_pushnil(L);
    return 1;
}

static int l_match(lua_State *L) {
    string haystack = luaL_checkstring(L, 1);
    string needle = luaL_checkstring(L, 2);
    int x;
    if(needle.size() > 0 && haystack.size() > 0) {
        boost::regex re(needle);
        boost::smatch match;
        if(boost::regex_search(haystack, match, re, boost::match_not_dot_newline)) {
            for(x=1; x < match.size(); x++) {
                lua_pushstring(L, string(match[x]).data());
            }
            return x-1;
        }
    }
    lua_pushnil(L);
    return 1;
}

static const luaL_Reg nyx_symbols[] = {
    {"gmatch", l_gmatch},
    {"match", l_match},
    {"replace", l_replace},
    {NULL, NULL}
};

int luaopen_pcre(lua_State *L) {
    luaL_register(L, "pcre", nyx_symbols);
    return 1;
}
