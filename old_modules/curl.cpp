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
#include "curl.hpp"

using namespace std;

static enum CurlSetValues { CS_none, CS_debug, CS_url, CS_follow,
	CS_referel, CS_cookiefile, CS_proxy, CS_proxy_usrpwd, CS_post,
	CS_usrpwd, CS_agent, CS_tofile };
static          map < string,
                CurlSetValues > CurlSets;

static int
l_curlnew(lua_State * L)
{
    Curl          **c = (Curl **) lua_newuserdata(L, sizeof(Curl *));
    *c = new Curl();
    luaL_getmetatable(L, "nyx.curl");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    return 1;
}

static int
curl_gc(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    if (c)
	delete          c;
    return 0;
}

static int
curl_test(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    c->set_url("http://www.google.com");
    string          page = c->fetch();
    lua_pushlstring(L, page.data(), page.size());
    return 1;
}

static int
curl_post_clear(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    c->set_post();
    return 0;
}

static int
curl_fetch(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    string          page = c->fetch();
    lua_pushlstring(L, page.data(), page.size());
    lua_newtable(L);
    for (c->h_it = c->header.begin(); c->h_it != c->header.end();
	 c->h_it++) {
	lua_pushstring(L, string((*c->h_it).first).data());
	lua_pushstring(L, string((*c->h_it).second).data());
	lua_settable(L, -3);
    }
    return 2;
}

static int
curl_seturl(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    string          url(luaL_checkstring(L, 2));
    c->set_url(url);
    return 0;
}

static int
curl_encode(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    string          s(luaL_checkstring(L, 2));
    string          r = c->encode(s);
    lua_pushlstring(L, r.data(), r.size());
    return 1;
}

static int
curl_decode(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    string          s(luaL_checkstring(L, 2));
    string          r = c->decode(s);
    lua_pushlstring(L, r.data(), r.size());
    return 1;
}

static int
curl_set(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    string          key(luaL_checkstring(L, 2));
    string          value_s,
                    value_s2;
    int             value_i;
    bool            value_b;
    switch (CurlSets[key]) {
    case CS_debug:
	value_i = luaL_checkinteger(L, 3);
	if (value_i)
	    value_b = true;
	else
	    value_b = false;
	if (value_b)
	    c->debug_on();
	else
	    c->debug_off();
	break;
    case CS_url:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_url(value_s);
	break;
    case CS_follow:
	value_i = luaL_checkinteger(L, 3);
	if (value_i)
	    value_b = true;
	else
	    value_b = false;
	if (value_b)
	    c->set_followlocation(true);
	else
	    c->set_followlocation(false);
	break;
    case CS_referel:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_referel(value_s);
	else
	    c->set_referel();
	break;
    case CS_cookiefile:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_cookie_file(value_s);
	else
	    c->set_cookie_file();
	break;
    case CS_proxy:
	value_s = luaL_checkstring(L, 3);
	value_s2 = luaL_checkstring(L, 4);
	if (value_s2.size() >> 0 && value_s.size() >> 0) {
	    if (value_s2 == "HTTP")
		value_i = CURLPROXY_HTTP;
	    else if (value_s2 == "SOCKS5")
		value_i = CURLPROXY_SOCKS5;
	    else if (value_s2 == "SOCKS4")
		value_i = CURLPROXY_SOCKS4;
	    else if (value_s2 == "SOCKS4A")
		value_i = CURLPROXY_SOCKS4A;
	    else
		value_i = CURLPROXY_HTTP;	// default it to HTTP if
						// unknown value
	    c->set_proxy(value_s, value_i);
	} else {
	    if (value_s.size() >> 0)
		c->set_proxy(value_s);
	    else
		c->set_proxy();
	}
	break;
    case CS_proxy_usrpwd:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_proxy_usrpwd(value_s);
	else
	    c->set_proxy_usrpwd();
	break;
    case CS_post:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_post(value_s);
	else
	    c->set_post();
	break;
    case CS_usrpwd:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_userpwd(value_s);
	else
	    c->set_userpwd();
	break;
    case CS_agent:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_agent(value_s);
	else
	    c->set_agent();
	break;
    case CS_tofile:
	value_s = luaL_checkstring(L, 3);
	if (value_s.size() > 0)
	    c->set_tofile(value_s);
	else
	    c->set_tofile();
	break;
    default:
	break;
    }
    return 0;
}

static int
curl_form_string(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    string          n(luaL_checkstring(L, 2));
    string          v(luaL_checkstring(L, 3));
    if (n.size() > 0) {		// sometimes contents are empty
	c->set_post_string(n, v);
    }
    return 0;
}

static int
curl_form_file(lua_State * L)
{
    Curl           *c = *(Curl **) luaL_checkudata(L, 1, "nyx.curl");
    string          n(luaL_checkstring(L, 2)),
                    f(luaL_checkstring(L, 3));
    if (n.size() > 0 && f.size() > 0) {
	if (lua_isstring(L, 4)) {	// We are doing it from memory
	    c->set_post_file_mem(n.data(), f.data(),
				 luaL_checkstring(L, 4), lua_objlen(L, 4));
	} else {		// from a local file
	    c->set_post_file(n.data(), f.data());
	}
    }
    return 0;
}

static const luaL_Reg curlf[] = {
    {"new", l_curlnew},
    {NULL, NULL}		// End
};

static const luaL_Reg curlm[] = {
    {"__gc", curl_gc},
    {"url", curl_seturl},
    {"set", curl_set},
    {"form_string", curl_form_string},
    {"form_file", curl_form_file},
    {"post_clear", curl_post_clear},
    {"encode", curl_encode},
    {"decode", curl_decode},
    {"fetch", curl_fetch},
    {NULL, NULL}		// End
};

int
luaopen_curl(lua_State * L)
{
    luaL_newmetatable(L, "nyx.curl");
    luaL_register(L, NULL, curlm);
    luaL_register(L, "curl", curlf);
    // Curl Stuff
    CurlSets["debug"] = CS_debug;
    CurlSets["url"] = CS_url;
    CurlSets["follow"] = CS_follow;
    CurlSets["referel"] = CS_referel;
    CurlSets["cookiefile"] = CS_cookiefile;
    CurlSets["proxy"] = CS_proxy;
    CurlSets["proxy_usrpwd"] = CS_proxy_usrpwd;
    CurlSets["post"] = CS_post;
    CurlSets["usrpwd"] = CS_usrpwd;
    CurlSets["agent"] = CS_agent;
    CurlSets["to_file"] = CS_tofile;
    return 1;
}
