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

/* Encoding/Decoding Tables */
static unsigned char b32e_crockford[] = "0123456789abcdefghjkmnpqrstvwxyz=";
static unsigned char b32d_crockford[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0,10,11,12,13,14,15,16,17, 0,18,19, 0,20,21, 0, 22,23,24,25,26,0,27,28,29,30,31, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0,10,11,12,13,14,15,16,17, 0,18,19, 0,20,21, 0, 22,23,24,25,26,0,27,28,29,30,31, 0, 0, 0, 0, 0,
/*                                                      �  �                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_rfc4648[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
static unsigned char b32d_rfc4648[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,26,27,28,29,30,31, 0, 0, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
/*                                                      �  �                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_zbase32[] = "ybndrfg8ejkmcpqxot1uwisza345h769\0";
static unsigned char b32d_zbase32[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,18, 0,25,26,27,30,29, 7,31, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0,24, 1,12, 3, 8, 5, 6,28,21, 9,10, 0,11, 2,16,13,14, 4,22,17,19, 0,20,15, 0,23, 0, 0, 0, 0, 0,
/*                                                      �  �                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_triacontakia[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV=";
static unsigned char b32d_triacontakia[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*                                                      �  �                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char b32e_nintendo[] = "0123456789BCDFGHJKLMNPQRSTVWXYZ.=";
static unsigned char b32d_nintendo[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*	SP !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,31, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
/*      @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _  */
	0, 0,10,11,12, 0,13,14,15, 0,16,17,18,19,20, 0,21,22,23,24,25, 0,26,27,28,29,30, 0, 0, 0, 0, 0,
/*      `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~     */
	0, 0,10,11,12, 0,13,14,15, 0,16,17,18,19,20, 0,21,22,23,24,25, 0,26,27,28,29,30, 0, 0, 0, 0, 0,
/*                                                      �  �                                            */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Encoding/Decoding Tables */

char *b32e_5(unsigned char *in, int len, unsigned char *out, unsigned char *table) { // in = 5 bytes, out = 8 bytes
	int x, y;
	uint64_t p, t=0;
	char pad = table[32];
	for(x=0, y=4; x < 5; ++x, --y) {
		p = in[x];
		t += p << ((+y)*8);
	}
	for(x=0, y=7; x < 8; ++x, --y) {
		out[x] = table[((t >> ((+y)*5)) & 0x1F)];
	}
	if(len<5)
		switch(len) { // why? because
			case 4: // 1
				out[7] = pad;
				break;
			case 3: // 3
				out[7] = pad;
				out[6] = pad;
				out[5] = pad;
				break;
			case 2: // 4
				out[7] = pad;
				out[6] = pad;
				out[5] = pad;
				out[4] = pad;
				break;
			case 1: // 6
				out[7] = pad;
				out[6] = pad;
				out[5] = pad;
				out[4] = pad;
				out[3] = pad;
				out[2] = pad;
				break;
			default:
				break;
		}
	return out;
}

char *b32d_8(unsigned char *in, unsigned char *out, unsigned char *tabled, unsigned char *tablee) { // in = 8 bytes, out = 5 bytes
	int x, y;
	uint64_t t=0, a;
	char pad = tablee[32];
	for(x=0; x < 8; ++x)
		if(in[x] == pad)
			in[x] = 0; // convert padding to zero;
	for(x=0, y=7; x < 8; ++x, --y) {
		a = tabled[in[x]];
		t += (a << ((+y)*5));
	}
	for(x=0, y=4; x < 5; ++x, --y)
		out[x] = (t >> ((+y)*8));
	return out;
}

int base32_ischarmap(char c, char *map) {
	int x, l=strlen(map);
	for(x=0; x < l; ++x)
		if(c == *(map+x))
			return 1;
	return 0;
}

char *base32_create_decode_table(char *encode_table, char *out) { // create simple decode table from char set, case sensitive
	int x;
	for(x=0; x < 32; ++x)
		out[encode_table[x]] = x;
	return out;
}

char *b32e(char *in, int len, char *out, char *table, int *c) {
	int x, y;
	for(x=0, y=0; x < len; x+=5, y+=8)
		b32e_5(in+x, len-x, out+y, table);
	*c = y;
	return out;
}

char *b32d(char *in, int len, char *out, char *tabled, char *tablee, int *c) {
	int x=0, y=0, z=0, w=0;
	char que[8];
	memset(que, 0, 8);
	for(x=0; x < len; ++x) {
		if(base32_ischarmap(*(in+x), tablee)) {
			que[z] = *(in+x); ++z;
			if(z==8) {
				b32d_8(que, out+y, tabled, tablee);
				y+=5; z=0;
				for(w=0; w < 8; ++w)
					que[w] = 0;
			}
		}
	}
	*c = y;
	if(z>0) { // Incomplete que to proccess / no = padding
		b32d_8(que, out+y, tabled, tablee);
		*c += 5;
	}
	return out;
}



static int base32_mapencode(lua_State *L, char *table) {
	int s=0, y=0;
	char *inbuf, *outbuf, *enc = luaL_checklstring(L, 1, &s);
	outbuf = calloc(s*2, sizeof(char));
	inbuf = calloc(s+5, sizeof(char));
	if(!outbuf || !inbuf) {
		if(outbuf)
			free(outbuf);
		if(inbuf)
			free(inbuf);
		luaL_error(L, "base32_mapencode: calloc() failed");
	}
	memcpy(inbuf, enc, s);
	b32e(inbuf, s, outbuf, table, &y);
	lua_pushlstring(L, outbuf, y);
	free(inbuf);
	free(outbuf);
	return 1;
}

static int base32_mapdecode(lua_State *L, char *tabled, char *tablee) {
	int s=0, y=0;
	char *outbuf, *dec = luaL_checklstring(L, 1, &s);
	outbuf = calloc(s, sizeof(char));
	if(!outbuf)
		luaL_error(L, "base32_mapdecode: calloc() failed");
	b32d(dec, s, outbuf, tabled, tablee, &y);
	lua_pushlstring(L, outbuf, y);
	free(outbuf);
	return 1;
}

/* Wrapper Calls */
static int base32_encode(lua_State *L) {
	return base32_mapencode(L, b32e_rfc4648);
}

static int base32_decode(lua_State *L) {
	return base32_mapdecode(L, b32d_rfc4648, b32e_rfc4648);
}

static int base32_encode_crockford(lua_State *L) {
	return base32_mapencode(L, b32e_crockford);
}

static int base32_decode_crockford(lua_State *L) {
	return base32_mapdecode(L, b32d_crockford, b32e_crockford);
}

static int base32_encode_zbase32(lua_State *L) {
	return base32_mapencode(L, b32e_zbase32);
}

static int base32_decode_zbase32(lua_State *L) {
	return base32_mapdecode(L, b32d_zbase32, b32e_zbase32);
}

static int base32_encode_triacontakia(lua_State *L) {
	return base32_mapencode(L, b32e_triacontakia);
}

static int base32_decode_triacontakia(lua_State *L) {
	return base32_mapdecode(L, b32d_triacontakia, b32e_triacontakia);
}

static int base32_encode_nintendo(lua_State *L) {
	return base32_mapencode(L, b32e_nintendo);
}

static int base32_decode_nintendo(lua_State *L) {
	return base32_mapdecode(L, b32d_nintendo, b32e_nintendo);
}

static int base32_encode_custom(lua_State *L) {
	int s=0;
	char tablee[34], *b=luaL_checklstring(L, 2, &s);
	if(s<32)
		luaL_error(L, "base32:custom_encode wrong map size");
	memset(tablee, 0, 34);
	memcpy(tablee, b, 32);
	if(s>32)
		*(tablee+32) = *(b+32); // set padding char
	s=base32_mapencode(L, tablee);
	return s;
}

static int base32_decode_custom(lua_State *L) {
	int s=0;
	char tabled[256], tablee[34], *b=luaL_checklstring(L, 2, &s);
	if(s<32)
		luaL_error(L, "base32:custom_decode wrong map size");
	memset(tabled, 0, 256);
	memset(tablee, 0, 34);
	memcpy(tablee, b, 32);
	if(s>32)
		*(tablee+32) = *(b+32); // set padding char
	base32_create_decode_table(tablee, tabled);
	s=base32_mapdecode(L, tabled, tablee);
	return s;
}

static int base32_wrap(lua_State *L) {
	int y=0, x=0, s=0, wrap=luaL_checknumber(L, 2);
	char *b=luaL_checklstring(L, 1, &s);
	luaL_Buffer buff;
	luaL_buffinit(L, &buff);
	for(x=0; x < s; ++x) {
		if(y==wrap) {
			luaL_addchar(&buff, *(b+x));
			luaL_addstring(&buff, "\n");
			y=0;
		} else {
			luaL_addchar(&buff, *(b+x));
			++y;
		}
	}
	luaL_pushresult(&buff);
	return 1;
}

static const luaL_Reg base32_r[] = {
	{"encode", base32_encode},
	{"decode", base32_decode},
	{"encode_rfc4648", base32_encode},
	{"decode_rfc4648", base32_decode},
	{"crockford_encode", base32_encode_crockford},
	{"crockford_decode", base32_decode_crockford},
	{"zbase32_encode", base32_encode_zbase32},
	{"zbase32_decode", base32_decode_zbase32},
	{"triacontakia_encode", base32_encode_triacontakia},
	{"triacontakia_decode", base32_decode_triacontakia},
	{"nintendo_encode", base32_encode_nintendo},
	{"nintendo_decode", base32_decode_nintendo},
	{"custom_encode", base32_encode_custom},
	{"custom_decode", base32_decode_custom},
	{"wrap", base32_wrap},
	{NULL, NULL}
};

int luaopen_base32(lua_State *L) {
	luaL_register(L, "base32", base32_r);
	return 1;
}
