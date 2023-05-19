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

using namespace std;

class Lua_Thread {
    private:
        LuaVM L;
        vector<map<string,string>> messages_tothread;
        vector<map<string,string>> messages_frommain;
        boost::mutex lock;
        bool is_thread(lua_State *L1) {
            if(L.getstate() == L1)
                return true;
            else
                return false;
        }
    public:
        Lua_Thread() {
        }
        void send_msg(lua_State *L1, map<string,string> m) {
            boost::lock_guard<boost::mutex> lg(lock);
            if(is_thread(L1)) {
            } else {
            }
        }
        map<string,string> recv_msg(lua_State *L1) {
            boost::lock_guard<boost::mutex> lg(lock);
            if(is_thread(L1)) {
            } else {
            }
        }
        ~Lua_Thread() {
        }
};

static int l_threadsnew(lua_State *L) {

}

static int l_threads_sendmsg(lua_State *L) {

}

static int l_threads_getmsg(lua_State *L) {

}

static int threads_gc(lua_State *L) {

}

static const luaL_Reg threadsf[] = {
	{"new", l_threadsnew},
	{NULL, NULL} // End
};

static const luaL_Reg threadsm[] = {
    {"__gc", threads_gc},
	{"get", l_threads_sendmsg},
	{"send", l_threads_getmsg},
	{NULL, NULL} // End
};

int luaopen_threads(lua_State *L) {
    return 1;
}
