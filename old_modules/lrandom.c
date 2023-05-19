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
#include <windows.h>
#include <pstdint.h>

HMODULE *Advapi32_h=NULL;
typedef int (WINAPI *RTLGENRANDOM_TYPE)(void *, uint64_t);
RTLGENRANDOM_TYPE RtlGenRandom=NULL;

int random_number(lua_State *L) {
	uint32_t n=0;
	(*RtlGenRandom)(&n, sizeof(uint32_t)); // Obviously 4 
	lua_pushnumber(L, n);
	return 1;
}

int random_binary(lua_State *L) {
	int n = luaL_checknumber(L, 1);
	char *b=calloc(n, sizeof(char));
	(*RtlGenRandom)(b, n);
	lua_pushlstring(L, b, n);
	free(b);
	return 1;
}

static const luaL_Reg random_r[] = {
	{"number", random_number},
	{"binary", random_binary},
	{NULL, NULL}
};

int luaopen_random(lua_State *L) {
	HMODULE h = GetModuleHandle("Advapi32.dll");
	if(!h) { // Is not loaded
		h = LoadLibrary("c:\\windows\\system32\\Advapi32.dll"); // FreeLibrary() is not called on purpose / stays loaded
		if(!h)
			luaL_error(L, "random: Can't load Advapi32.dll");
	}
	RtlGenRandom = (RTLGENRANDOM_TYPE)GetProcAddress(h, "SystemFunction036");
	if(!RtlGenRandom)
		luaL_error(L, "random: SystemFunction036 Missing?!?!");
	luaL_register(L, "random", random_r);
	return 1;
}
