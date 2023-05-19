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

#ifndef _MAIN_H_
#define _MAIN_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <algorithm>
//#include <cstdint>
#include <boost/cstdint.hpp>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/cstdint.hpp>

using namespace std;
using namespace boost;

extern "C" {
#include "lua-5.1.4/src/lua.h"
#include "lua-5.1.4/src/lualib.h"
#include "lua-5.1.4/src/lauxlib.h"
#include "luasocket-2.0.2/src/luasocket.h"
//#include "luafilesystem-1.4.2/src/lfs.h"

#include "lzma\LzmaEnc.h"
#include "lzma\LzmaDec.h"
#include "lzma\Alloc.h"
#include "lzma\7zCrc.h"
}

#include "b64.hpp"
#include "adler32.hpp"
#include "md5.hpp"
#include "sha1.hpp"
#include "lua.hpp"
#include "wxNyx.hpp"

void luaNyx_register(lua_State *L, const luaL_Reg *l);

int luaopen_curl(lua_State *L);
int luaopen_pcre(lua_State *L);
int luaopen_nyx(lua_State *L);
int luaopen_dir(lua_State *L);
int luaopen_uuid(lua_State *L);
int luaopen_sqlite3(lua_State *L);
int luaopen_rand(lua_State *L);
int luaopen_lzma(lua_State *L);

int luaopen_io_nyx(lua_State *L);
string iob_getline();
void iob_putline(string s);
map<int,string> iob_getarray();

#endif
