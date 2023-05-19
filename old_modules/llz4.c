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
#include "lz4.h"
#include "lz4hc.h"

/* data, size = lz4.compress("some stuff") */
static int l_lz4_compress(lua_State *L) {
	int s=0;
	char *comp=NULL, *buff = luaL_checklstring(L, 1, &s);
	comp = calloc(LZ4_compressBound(s), sizeof(char));
	if(!comp)
		luaL_error(L, "lz4: calloc() failed");
	s=LZ4_compress(buff, comp, s);
	lua_pushlstring(L, comp, s);
	lua_pushnumber(L, s);
	free(comp);
	return 2;
}

/* data, size = lz4.compressHC("some stuff") */
static int l_lz4_compressHC(lua_State *L) {
	int s=0;
	char *comp=NULL, *buff = luaL_checklstring(L, 1, &s);
	comp=calloc(LZ4_compressBound(s), sizeof(char));
	if(!comp)
		luaL_error(L, "lz4: calloc() failed");
	s=LZ4_compressHC(buff, comp, s);
	lua_pushlstring(L, comp, s);
	lua_pushnumber(L, s);
	free(comp);
	return 2;
}

/* data, size = lz4.decompress("some stuff") */
static int l_lz4_decompress(lua_State *L) {
	int s=0;
	char *decomp=NULL, *buff = luaL_checklstring(L, 1, &s);
	decomp=calloc(s*4, sizeof(char));
	if(!decomp)
		luaL_error(L, "lz4: calloc() failed");
	s=LZ4_uncompress_unknownOutputSize(buff, decomp, s, s*4);
	if(s<0) {
		free(decomp);
		luaL_error(L, "lz4: malformed or invalid compression stream");
	}
	lua_pushlstring(L, decomp, s);
	lua_pushnumber(L, s);
	free(decomp);
	return 2;
}

static const luaL_Reg lz4_r[] = {
	{"compress", l_lz4_compress},
	{"compressHC", l_lz4_compressHC},
	{"decompress", l_lz4_decompress},
	{NULL, NULL}
};

int luaopen_lz4(lua_State *L) {
	luaL_register(L, "lz4", lz4_r);
	return 1;
}
