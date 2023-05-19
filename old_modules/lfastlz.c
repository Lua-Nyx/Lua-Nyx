/*
* Copyright (C) 2013, William H. Welna All rights reserved.
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

#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "fastlz.h"

/* data, size = fastlz.compress(1, "Something To Compress") */
static int l_fastlz_compress(lua_State *L) {
	int s=0, level = luaL_checknumber(L, 1);
	char *comp=NULL, *buff = luaL_checklstring(L, 2, &s);
	if(s < 66)
		luaL_error(L, "fastlz: input buffer smaller than 66 bytes");
	comp=calloc((s+(s/20)), sizeof(char));
	if(!comp)
		luaL_error(L, "fastlz: calloc() failed");
	s=fastlz_compress_level(level, buff, s, comp);
	if(s<=0) {
		free(comp);
		luaL_error(L, "fastlz: fastlz_compress() failed");
	}
	lua_pushlstring(L, comp, s);
	lua_pushnumber(L, s);
	free(comp);
	return 2;
}

/* data, size = fastlz.decompress("data data") */
static int l_fastlz_decompress(lua_State *L) {
	int s=0;
	char *decomp, *buff = luaL_checklstring(L, 1, &s);
	decomp=calloc(s*4, sizeof(char));
	if(!decomp)
		luaL_error(L, "fastlz: calloc() failed");
	s=fastlz_decompress(buff, s, decomp, s*4);
	if(!s) {
		free(decomp);
		luaL_error(L, "fastlz: fastlz_decompress() failed");
	}
	lua_pushlstring(L, decomp, s);
	lua_pushnumber(L, s);
	free(decomp);
	return 2;
}

static const luaL_Reg fastlz_r[] = {
	{"compress", l_fastlz_compress},
	{"decompress", l_fastlz_decompress},
	{NULL, NULL}
};

int luaopen_fastlz(lua_State *L) {
	luaL_register(L, "fastlz", fastlz_r);
	return 1;
}
