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

#ifndef __LUAHPP__
#define __LUAHPP__

#include "main.hpp"

using namespace std;

typedef struct {
    const char *data;
    int size;
    bool loaded;
} loadmem_struct;

typedef struct {
    const char *data;
    int size;
    int crc32sum;
    char *lzma_data;
    char lzma_props[LZMA_PROPS_SIZE];
    int lzma_props_size;
    int lzma_size;
    int lzma_crc32sum;
} lua_compiled_chunk;

typedef struct {
    uint8_t magick[16];
    uint32_t crc32sum_compressed;
    uint64_t size_compressed;
    uint32_t crc32sum_uncompressed;
    uint64_t size_uncompressed;
    uint8_t lzma_props[LZMA_PROPS_SIZE];
    uint8_t reserved[8];
} NyxFileHeader;

class LuaVM {
    private:
    lua_State *L;
    int lzma_compress(lua_compiled_chunk *c);
    int lzma_decompress(lua_compiled_chunk *c);
    void qxore(char *d, int size, char *k);
    void qxord(char *d, int size, char *k);
    void openlibs();
    public:
    LuaVM();
    LuaVM(string code);
    LuaVM(lua_State *S);
    LuaVM(lua_State *S, string code);
    // Utility Functions
    lua_State *getstate();
    void register_function(string name, lua_CFunction fn);
    lua_compiled_chunk *compile(string name, const char *b, int size);
    bool run_chunk(lua_compiled_chunk *c, string name, bool isCompressed);
    void chunk2file(lua_compiled_chunk *c, string filename);
    lua_compiled_chunk *file2chunk(string filename);
    bool runfile(string filename);
    bool runfile_frommem(string name, char *b, int size);
    void compile_free(lua_compiled_chunk *c);
    bool loadfile(string name);
    bool loadmem(string name, const char *b, int size);
    bool loadmem(string name, string code);
    bool checkstack(int free);
    void call(int nargs, int nresults);
    bool call(lua_CFunction f, void *ud);
    int gc(int what, int data);
    // Table Stuff
    void createtable(int narr, int nrec);
    void newtable();
    void getfield(int i, string k);
    void gettable(int i);
    // Misc
    void getglobal(string name);
    void concat(int n);
    size_t objlen(int i);
    bool equal(int i1, int i2);
    bool lessthan(int i1, int i2);
    // Stack
    int gettop();
    void insert(int i);
    void pop(int i);
    void pop();
    void copy(int i);
    // type checks
    bool isboolean(int i);
    bool iscfunction(int i);
    bool isfunction(int i);
    bool isnil(int i);
    bool isnone(int i);
    bool isnoneornil(int i);
    bool isnumber(int i);
    bool isstring(int i);
    bool istable(int i);
    bool isthread(int i);
    bool isuserdata(int i);
    bool islightuserdata(int i);
    // Push
    void push(bool b);
    void push(lua_CFunction fn, int n);
    void push(lua_CFunction fn);
    void push(int i);
    void push(void *n);
    void push(string s);
    void push(char *s, size_t len);
    void push(char *s);
    void push(); // nil
    // Push Table
    void pushtable(string key, string value);
    void pushtable(int key, string value);
    void pushtable(int key, int value);
    void pushtable(string key, int value);
    // Destructor
    ~LuaVM();
};

#endif
