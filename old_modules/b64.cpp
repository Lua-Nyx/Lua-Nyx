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

uint8_t *BASE64::tri_encode(uint8_t *b, int l, uint8_t *r) {
	const char *b64e = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	uint32_t t;
	t = (b[0] << 16) + (b[1] << 8) + (b[2]);
	r[0] = b64e[(t >>18) & 0x3f];
	r[1] = b64e[(t >>12) & 0x3f];
	r[2] = b64e[(t >>6) & 0x3f];
	r[3] = b64e[(t) & 0x3f];
	if(l == 2)
		r[3] = '=';
	if(l == 1)
		r[3] = r[2] = '=';
	return r;
}

uint8_t *BASE64::quad_decode(uint8_t *b, uint8_t *r) {
	uint32_t t, x;
	const uint8_t b64d[] = {
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,
	    0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
	    0,0,0,0,0,0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
	    45,46,47,48,49,50,51,0
	};
	for(x=0; x < 4; x++)
		if(b[x] == '=')
			b[x] = 0;
	t = (b64d[b[0]] << 18) + (b64d[b[1]] << 12) + (b64d[b[2]] << 6) + (b64d[b[3]]);
	cout << t << endl;
	r[0] = t >> 16;
	r[1] = t >> 8;
	r[2] = t;
	cout << (uint32_t)r[0] << " " << (uint32_t)r[1] << " " << (uint32_t)r[2] << endl;
	return r;
}

char *BASE64::encode_string(char *in, int in_len, char *out, int out_len) {
	int x;
	// Sanitize out buffer
	for(x=0; x < out_len; x++)
		out[x]=0;
	for(x=0; (x < in_len) && (x < out_len+1); x+=3) {
		if((in_len - x) <= 2)
			tri_encode((uint8_t *)in+x, (in_len-x), (uint8_t *)out+x);
		else
			tri_encode((uint8_t *)in+x, 3, (uint8_t *)out+x);
	}
	return out;
}

char *BASE64::decode_string(char *in, int in_len, char *out, int out_len) {
	int x;
	char *p = out;
	// Sanitize out buffer
	for(x=0; x < out_len; x++)
		out[x]=0;
	for(x=0; (x < in_len) && (x < out_len+1); x+=4, in+=4, out+=3) {
		cout << (void *)out << endl;
		quad_decode((uint8_t *)in, (uint8_t *)out);
	}
	cout << (uint32_t)p[0] << endl;
	return p;
}
