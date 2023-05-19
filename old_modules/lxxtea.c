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
#include <pstdint.h>

/* - XXTEA by David Wheeler and Roger Needham - */
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

void btea(uint32_t *v, int n, uint32_t const key[4]) {
	uint32_t y, z, sum;
	unsigned p, rounds, e;
	if (n > 1) {          /* Coding Part */
		rounds = 6 + 52/n;
		sum = 0;
		z = v[n-1];
		do {
			sum += DELTA;
			e = (sum >> 2) & 3;
			for (p=0; p<n-1; p++) {
				y = v[p+1];
				z = v[p] += MX;
			}
			y = v[0];
			z = v[n-1] += MX;
		} while (--rounds);
	} else if (n < -1) {  /* Decoding Part */
		n = -n;
		rounds = 6 + 52/n;
		sum = rounds*DELTA;
		y = v[0];
		do {
			e = (sum >> 2) & 3;
			for (p=n-1; p>0; p--) {
				z = v[p-1];
				y = v[p] -= MX;
			}
			z = v[n-1];
			y = v[0] -= MX;
		} while ((sum -= DELTA) != 0);
    }
}
/* - XXTEA by David Wheeler and Roger Needham - */



void xxtea_magick(lua_State *L, int f) {
	int keylen=0, blocklen=0;
	char *b=NULL;
	char *key=(char *)luaL_checklstring(L, 1, &keylen);
	char *block=(char *)luaL_checklstring(L, 2, &blocklen);
	if(keylen != 16)
		luaL_error(L, "xxtea: Invalid key size");
	if(blocklen<8)
		luaL_error(L, "xxtea: Block less than 8 bytes");
	if(!(b=calloc(blocklen+4, sizeof(char))))
		luaL_error(L, "xxtea: Allocation Error");
	if(blocklen%4)
		luaL_error(L, "xxtea: Block size must be a multiple of 4");
	memcpy(b, block, blocklen);
	if(f)
		btea((uint32_t *)b, blocklen/4, (uint32_t *)key);
	else
		btea((uint32_t *)b, ((0xFFFFFFFF-(blocklen/4))+1), (uint32_t *)key);
	lua_pushlstring(L, b, blocklen);
	free(b);
}

/* xxtea.encrypt(key, block) */
static int xxtea_encrypt(lua_State *L) {
	xxtea_magick(L, 1);
	return 1;
}

/* xxtea.decrypt(key, block) */
static int xxtea_decrypt(lua_State *L) {
	xxtea_magick(L, 0);
	return 1;
}

static const luaL_Reg xxtea_r[] = {
	{"encrypt", xxtea_encrypt},
	{"decrypt", xxtea_decrypt},
	{NULL, NULL}
};

int luaopen_xxtea(lua_State *L) {
	luaL_register(L, "xxtea", xxtea_r);
	return 1;
}
