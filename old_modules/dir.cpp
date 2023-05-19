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
using namespace boost::filesystem;
using boost::format;

template<class T> string getsum(string filename) {
    ifstream in(filename.data(), ios::binary);
    if(in.is_open()) {
        uint8_t *sum_raw;
        string sum_string; // Hex string of file sum
        T hash = T();
        char *buffer = new char[4096];
        int s, x;
        while(in.good()) {
            in.read(buffer, 4096);
            s = in.gcount();
            hash.update((char *)buffer, s);
        }
        sum_raw = (uint8_t *)hash.sum();
        for(x=0; x != hash.digest_size(); x++) {
            sprintf(buffer, "%02x", sum_raw[x]);
            sum_string.append(buffer);
        }
        in.close();
        delete[] buffer;
        return sum_string;
    } else
        return string("");
}

class Dir {
    private:
        path dir;
        path last_file;
        vector<path> files;
        vector<path>::iterator it;
        bool error;
    public:
        Dir(string p) {
            dir = path(p);
            error=false;
            if(is_directory(p))
                copy(directory_iterator(dir), directory_iterator(), back_inserter(files));
            else
                error=true;
            it = vector<path>::iterator(files.begin());
        }
        string get_file() { // return next file or nothing
            if(it != files.end()) {
                string r((*it).string()); last_file = (*it); ++it;
                if(r != "." && r != "..")
                    return r;
                else
                    return string("");
            } else
                return string("");
        }
        map<string,string> file_info(string f="") {
            path file;
            if(f.size() > 0)
                file = path(f);
            else
                file = last_file;
            map<string,string> info; // table of file information
            if(exists(file)) {
                info["path"] = file.string();
                info["root name"] = file.root_name().string();
                info["root directory"] = file.root_directory().string();
                info["root path"] = file.root_path().string();
                info["relative path"] = file.relative_path().string();
                info["parent path"] = file.parent_path().string();
                info["filename"] = file.filename().string();
                info["stem"] = file.stem().string();
                info["extension"] = file.extension().string();
                info["exists"] = string("true");
                info["error"] = string("false");
                if(is_directory(file)) {
                    cout << "Directory" << endl;
                    info["size"] = string("");
                    info["adler32"] = string("");
                    info["md5"] = string("");
                    info["sha1"] = string("");
                    info["is directory"] = string("true");
                } else {
                    try {
                        info["size"] = boost::lexical_cast<string>(file_size(file));
                    } catch(filesystem_error &ex) {
                        info["size"] = string("");
                    }
                    info["adler32"] = getsum<Adler32>(info["relative path"]);
                    info["md5"] = getsum<MD5>(info["relative path"]);
                    info["sha1"] = getsum<SHA1>(info["relative path"]);
                    info["is directory"] = string("false");
                }
            } else {
                info["exists"] = string("false");
            }
            return info;
        }
        ~Dir() {
        }
};

static int dir_i(lua_State *L) {
    Dir *d = *(Dir **)luaL_checkudata(L, lua_upvalueindex(1), "nyx.dir");
    string s;
    if((s=d->get_file()) != "") {
        map<string,string> i;
        try {
            i = d->file_info();
        } catch (std::exception &e) {
            cerr << "dir.nyx:dir_i " << e.what() << endl;
            i["error"] = string("true");
        }
        map<string,string>::iterator it;
        lua_newtable(L);
        for(it=i.begin(); it != i.end(); it++) {
            lua_pushstring(L, string((*it).first).data());
            lua_pushstring(L, string((*it).second).data());
            lua_settable(L, -3);
        }
        return 1;
    }
    return 0;
}

static int l_dir(lua_State *L) {
    Dir **d = (Dir **)lua_newuserdata(L, sizeof(Dir *));
    luaL_getmetatable(L, "nyx.dir");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    string p = string(luaL_checkstring(L, 1));
    if(p.size() > 0) {
        *d = new Dir(p);
        lua_pushcclosure(L, dir_i, 1);
        return 1;
    }
    return 0;
}

static int dir_gc(lua_State *L) {
    Dir *d = *(Dir **)luaL_checkudata(L, 1, "nyx.dir");
    if(d)
        delete d;
    return 0;
}

static const luaL_Reg dirm[] = {
    {"__gc", dir_gc},
    {NULL, NULL}
};

int luaopen_dir(lua_State *L) {
    luaL_newmetatable(L, "nyx.dir");
    luaL_register(L, NULL, dirm);
    lua_pushcfunction(L, l_dir);
    lua_setglobal(L, "dir");
    return 1;
}
