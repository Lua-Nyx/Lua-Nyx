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

static void *SzAlloc(void *p, size_t size) { p = p; return MyAlloc(size); }
static void SzFree(void *p, void *address) { p = p; MyFree(address); }
static ISzAlloc g_alloc = { SzAlloc, SzFree };

const char *lua_loadmem(lua_State *L, void *data, size_t *size) {
    loadmem_struct *d = (loadmem_struct *)data;
    if(d->loaded == false){
        *size = d->size;
        d->loaded = true;
        return (const char*)d->data;
    } else {
        *size=0;
        return NULL;
    }
}

int lua_writemem(lua_State *L, const void* p, size_t sz, void* ud) {
    lua_compiled_chunk *c = (lua_compiled_chunk *)ud;
    char *t = (char *)realloc((void *)c->data, sz+c->size);
    if(t) {
        memcpy((t+c->size), p, sz);
        c->data = (const char*)t;
    } else {
        return 1;
    }
    c->size = sz + c->size;
    return 0;
}

void lua_debug_compiled(lua_compiled_chunk *c) {
    fprintf(stderr, "<SIZE>%i</SIZE>\n<CRC>%x</CRC>\n\n", c->size, c->crc32sum);
    fprintf(stderr, "<LZMA_SIZE>%i</LZMA_SIZE>\n<LZMA_CRC>%x</LZMA_CRC>\n\n", c->lzma_size, c->lzma_crc32sum);
}

void LuaVM::openlibs() {
    try {
        luaL_openlibs(L);
        luaopen_socket_core(L);
        //luaopen_lfs(L);
        luaopen_curl(L);
        luaopen_pcre(L);
        luaopen_dir(L);
        luaopen_uuid(L);
        luaopen_sqlite3(L);
        luaopen_nyx(L);
        luaopen_rand(L);
        luaopen_io_nyx(L);
        luaopen_lzma(L);
        CrcGenerateTable();
    } catch (std::exception &e) {
        cerr << "LuaVM:openlibs " << e.what() << endl;
        exit(0);
    }
}

LuaVM::LuaVM() {
    L = luaL_newstate();
    openlibs();
}

LuaVM::LuaVM(string code) {
    L = luaL_newstate();
    openlibs();
    loadmem("LuaVM", code.data(), code.size());
}

LuaVM::LuaVM(lua_State *S) {
    L = S;
}

LuaVM::LuaVM(lua_State *S, string code) {
    L = S;
    loadmem("LuaVM", code.data(), code.size());
}

lua_State *LuaVM::getstate() {
    return L;
}
void LuaVM::register_function(string name, lua_CFunction fn) {
    lua_register(L, name.data(), fn);
}

void LuaVM::qxore(char *d, int size, char *k) {
    uint8_t k1=k[0], k2=k[1], k3=k[2], k4=k[3], x;
    for(x=0; x <= size; x++) {
        d[x] = ((((d[x]^k1)^k2)^k3)^k4);
        ++k4;
    }
}

void LuaVM::qxord(char *d, int size, char *k) {
    uint8_t k1=k[0], k2=k[1], k3=k[2], k4=k[3], x;
    for(x=0; x <= size; x++) {
        d[x] = ((((d[x]^k4)^k3)^k2)^k1);
        ++k4;
    }
}

int LuaVM::lzma_compress(lua_compiled_chunk *c) {
    CLzmaEncProps props;
    c->lzma_data = (char *)calloc(c->size, sizeof(char)); c->lzma_size = c->size;
    if(!c->lzma_data)
        return SZ_ERROR_MEM;
    LzmaEncProps_Init(&props);
    props.level = 9;
    c->lzma_props_size = LZMA_PROPS_SIZE;
    int ret = LzmaEncode((Byte *)c->lzma_data, (SizeT *)&c->lzma_size, (Byte *)c->data, (SizeT)c->size, &props, (Byte *)c->lzma_props, (SizeT *)&c->lzma_props_size, 0, NULL, &g_alloc, &g_alloc);
    qxore((char *)c->lzma_data, c->lzma_size, "KIKE");
    return ret;
}

int LuaVM::lzma_decompress(lua_compiled_chunk *c) {
    ELzmaStatus status;
    if(c->size <= 0)
        return SZ_ERROR_FAIL;
    c->data = (char *)calloc(c->size, sizeof(char));
    if(!c->data)
        return SZ_ERROR_MEM;
    c->lzma_props_size = LZMA_PROPS_SIZE;
    qxord((char *)c->lzma_data, c->lzma_size, "KIKE");
    return LzmaDecode((Byte *)c->data, (SizeT *)&c->size, (Byte *)c->lzma_data, (SizeT *)&c->lzma_size, (Byte *)c->lzma_props, (SizeT)c->lzma_props_size, LZMA_FINISH_ANY, &status, &g_alloc);
}

lua_compiled_chunk *LuaVM::compile(string name, const char *b, int size) {
    lua_compiled_chunk *c = new lua_compiled_chunk;
    loadmem_struct s;
    c->data = (char *)calloc(1, sizeof(char)); c->size = 0;
    c->lzma_data = NULL; c->lzma_size = 0;
    s.data = b; s.size = size; s.loaded = false;
    if(lua_load(L, &lua_loadmem, (void *)&s, name.data())) { // Load hunk
        cerr << "LuaVM Error: " << lua_tostring(L, -1) << endl;
        lua_pop(L, 1);
        compile_free(c);
        return NULL;
    }
    if(lua_dump(L, &lua_writemem, c)) {
        lua_pop(L, 1);
        compile_free(c);
        return NULL;
    } else
        lua_pop(L, 1); // pop the compiled chunk off the stack / not needed
    if(lzma_compress(c)!=SZ_OK) { // Compress the chunk
        cerr << "LuaVM Error: LZMA Compression Error" << endl;
        compile_free(c);
        return NULL;
    }
    c->crc32sum = (int)CrcCalc(c->data, (size_t)c->size);
    c->lzma_crc32sum = (int)CrcCalc(c->lzma_data, (size_t)c->lzma_size);
    //lua_debug_compiled(c);
    return c; // returned compiled chunk
}

bool LuaVM::run_chunk(lua_compiled_chunk *c, string name, bool isCompressed) {
    if(isCompressed) { // Decompress chunk before running
        if(lzma_decompress(c)==SZ_OK)
            return loadmem(name, c->data, c->size);
        else
            return false; // decompression error
    } else { // Not Compressed
        return loadmem(name, c->data, c->size);
    }
    return false;
}

void LuaVM::chunk2file(lua_compiled_chunk *c, string filename) {
    try {
        ofstream of(filename.data(), ofstream::binary);
        // Setup file header
        NyxFileHeader h;
        void *p = (void *)&h;
        memset(p, 0, sizeof(NyxFileHeader)); // Zero the entire header
        memcpy(h.magick, "Nyx1b\x0xxxxxxxxxx", 16);
        memset(h.reserved, 'x', 8);
        h.crc32sum_compressed = c->lzma_crc32sum;
        h.size_compressed = c->lzma_size;
        h.crc32sum_uncompressed = c->crc32sum;
        h.size_uncompressed = c->size;
        memcpy(h.lzma_props, c->lzma_props, LZMA_PROPS_SIZE);
        of.write((const char *)p, sizeof(NyxFileHeader));
        of.flush();
        of.write(c->lzma_data, c->lzma_size);
        of.flush();
        of.close();
        cout << sizeof(NyxFileHeader) << endl;
    } catch (std::exception &e) {
        cerr << "LuaVM: chunk2file exception " << e.what() << " " << filename << endl;
    }
}

lua_compiled_chunk *LuaVM::file2chunk(string filename) {
    try {
        ifstream ifs(filename.data(), ifstream::binary);
        NyxFileHeader h;
        void *p = (void *)&h;
        memset(p, 0, sizeof(NyxFileHeader)); // Zero the entire header
        ifs.read((char *)p, sizeof(NyxFileHeader));
        if(ifs.eof()) {
            cerr << "LuaVM: file2chunk header too short " << filename << endl;
            return NULL; // error weeee
        }
        if(memcmp(h.magick, "Nyx1b\x0xxxxxxxxxx", 16)!=0) {
            cerr << "LuaVM: Magick Doesn't Match " << filename << endl;
            return NULL;
        }
        char *b = (char *)calloc(h.size_compressed+1, sizeof(char));
        if(!b) {
            cerr << "LuaVM: file2chunk Allocation Error " << h.size_compressed << endl;
            return NULL;
        }
        ifs.read(b, h.size_compressed); // compressed data
        ifs.close(); // no longer needed
        lua_compiled_chunk *c = new lua_compiled_chunk;
        c->size = h.size_uncompressed;
        c->crc32sum = h.crc32sum_uncompressed;
        c->lzma_size = h.size_compressed;
        c->lzma_crc32sum = h.crc32sum_compressed;
        c->lzma_data = b;
        memcpy(c->lzma_props, h.lzma_props, LZMA_PROPS_SIZE);
        if(lzma_decompress(c)!=SZ_OK) {
            cerr << "LuaVM: file2chunk decompression fail " << filename << endl;
            compile_free(c);
            return NULL;
        }
        return c;
    } catch (std::exception &e) {
        cerr << "LuaVM: file2chunk exception " << e.what() << " " << filename << endl;
        return NULL;
    }
}

bool LuaVM::runfile(string filename) {
    lua_compiled_chunk *c = file2chunk(filename);
    if(c) {
        if(!run_chunk(c, filename, false)) {
            cerr << "LuaVM: runfile error " << filename << endl;
            compile_free(c);
            return false;
        }
        compile_free(c);
        return true;
    }
    compile_free(c);
    return false;
}

bool LuaVM::runfile_frommem(string name, char *b, int size) {
    try {
        NyxFileHeader *h = (NyxFileHeader *)b;
        if(memcmp(h->magick, "Nyx1b\x0xxxxxxxxxx", 16)!=0) {
            cerr << "LuaVM: runfile_frommem: Magick Doesn't Match" << endl;
            return false;
        }
        lua_compiled_chunk *c = new lua_compiled_chunk;
        c->lzma_data = (char *)calloc(h->size_compressed+1, sizeof(char));
        c->lzma_crc32sum = h->crc32sum_compressed;
        c->lzma_size = h->size_compressed;
        memcpy(c->lzma_data, b+sizeof(NyxFileHeader), h->size_compressed);
        memcpy(c->lzma_props, h->lzma_props, LZMA_PROPS_SIZE);
        run_chunk(c, name, true);
        compile_free(c); // free it up
    } catch(std::exception &e) {
        cerr << "LuaVM: " << endl;
        return false;
    }
    return true;
}

void LuaVM::compile_free(lua_compiled_chunk *c) {
    if(c) {
        if(c->data)
            free((void *)c->data);
        if(c->lzma_data)
            free((void *)c->lzma_data);
        delete c;
    }
}

bool LuaVM::loadfile(string name) {
    try {
        if(luaL_loadfile(L, name.data()) || lua_pcall(L, 0, 0, 0)) {
            cerr << "LuaVM:loadfile Error: " << lua_tostring(L, -1) << endl;
            lua_pop(L, 1);
            return false;
        }
    } catch (std::exception &e) {
        cerr << "LuaVM:loadfile " << e.what() << endl;
        exit(0);
    }
    return true;
}
    //c:\users\user\documents\nyx core\lua.hpp|41|error C2664: 'lua_load' : cannot convert parameter 2 from 'const char *(__thiscall LuaVM::* )(lua_State *,void *,size_t *)' to 'lua_Reader'|
    //c:\users\user\documents\nyx core\lua.hpp|42|error C2440: 'type cast' : cannot convert from 'const char *(__thiscall LuaVM::* )(lua_State *,void *,size_t *)' to 'lua_Reader'|
bool LuaVM::loadmem(string name, const char *b, int size) {
    try {
        loadmem_struct s;
        s.data = b; s.size = size; s.loaded = false;
        if(lua_load(L, &lua_loadmem, (void *)&s, name.data()) || lua_pcall(L, 0, 0, 0)) {
            cout << "LuaVM:loadmem Error: " << lua_tostring(L, -1) << endl;
            lua_pop(L, 1);
            return false;
        }
    } catch (std::exception &e) {
        cerr << "LuaVM:loadmem " << e.what() << endl;
        exit(0);
    }
    return true;
}

bool LuaVM::loadmem(string name, string code) {
    return loadmem(name, code.data(), code.size());
}

bool LuaVM::checkstack(int free) {
    if(lua_checkstack(L, free)) {
        return true;
    } else
        return false;
}

void LuaVM::call(int nargs, int nresults) {
    try {
        lua_call(L, nargs, nresults);
    } catch (std::exception &e) {
        cerr << "LuaVM:call " << e.what() << endl;
        exit(0);
    }
}

bool LuaVM::call(lua_CFunction f, void *ud) {
    try {
        if(lua_cpcall(L, f, ud)) {
            cout << "LuaVM:call Error: " << lua_tostring(L, -1) << endl;
            lua_pop(L, 1);
            return false;
        } else
            return true;
    } catch (std::exception &e) {
        cerr << "LuaVM:loadfile " << e.what() << endl;
        exit(0);
    }
}

int LuaVM::gc(int what, int data) {
    return lua_gc(L, what, data);
}

void LuaVM::createtable(int narr, int nrec) {
    lua_createtable(L, narr, nrec);
}

void LuaVM::newtable() {
    createtable(0, 0);
}

void LuaVM::getfield(int i, string k) {
    lua_getfield(L, i, k.data());
}

void LuaVM::gettable(int i) {
    lua_gettable(L, i);
}

void LuaVM::getglobal(string name) {
    lua_getglobal(L, name.data());
}

void LuaVM::concat(int n) {
    lua_concat(L, n);
}

size_t LuaVM::objlen(int i) {
    return lua_objlen(L, i);
}

bool LuaVM::equal(int i1, int i2) {
    if(lua_equal(L, i1, i2))
        return true;
    return false;
}

bool LuaVM::lessthan(int i1, int i2) {
    if(lua_lessthan(L, i1, i2))
        return true;
    return false;
}

int LuaVM::gettop() {
    return lua_gettop(L);
}

void LuaVM::insert(int i) {
    lua_insert(L, i);
}

void LuaVM::pop(int i) {
    lua_pop(L, i);
}

void LuaVM::pop() {
    lua_pop(L, 1);
}

void LuaVM::copy(int i) {
    lua_pushvalue(L, i);
}

// type checks
bool LuaVM::isboolean(int i) {
    if(lua_isboolean(L, i))
        return true;
    return false;
}

bool LuaVM::iscfunction(int i) {
    if(lua_iscfunction(L, i))
        return true;
    return false;
}

bool LuaVM::isfunction(int i) {
    if(lua_isfunction(L, i))
        return true;
    return false;
}

bool LuaVM::isnil(int i) {
    if(lua_isnil(L, i))
        return true;
    return false;
}

bool LuaVM::isnone(int i) {
    if(lua_isnone(L, i))
        return true;
    return false;
}

bool LuaVM::isnoneornil(int i) {
    if(lua_isnoneornil(L, i))
        return true;
    return false;
}

bool LuaVM::isnumber(int i) {
    if(lua_isnumber(L, i))
        return true;
    return false;
}

bool LuaVM::isstring(int i) {
    if(lua_isstring(L, i))
        return true;
    return false;
}

bool LuaVM::istable(int i) {
    if(lua_istable(L, i))
        return true;
    return false;
}

bool LuaVM::isthread(int i) {
    if(lua_isthread(L, i))
        return true;
    return false;
}

bool LuaVM::isuserdata(int i) {
    if(lua_isuserdata(L, i))
        return true;
    return false;
}

bool LuaVM::islightuserdata(int i) {
    if(lua_islightuserdata(L, i))
        return true;
    return false;
}

// Push functions
void LuaVM::push(bool b) {
    int i = b;
    lua_pushboolean(L, i);
}

void LuaVM::push(lua_CFunction fn, int n) {
    lua_pushcclosure(L, fn, n);
}

void LuaVM::push(lua_CFunction fn) {
    push(fn, 0);
}

void LuaVM::push(int i) {
    lua_pushinteger(L, i);
}

void LuaVM::push(void *n) {
    lua_pushlightuserdata(L, n);
}

void LuaVM::push(string s) {
    lua_pushlstring(L, s.data(), s.size());
}

void LuaVM::push(char *s, size_t len) {
    lua_pushlstring(L, (const char *)s, len);
}

void LuaVM::push(char *s) {
    lua_pushstring(L, s);
}

void LuaVM::push() {
    lua_pushnil(L);
}

    // Table Functions
void LuaVM::pushtable(string key, string value) {
    lua_pushstring(L, key.data());
    lua_pushstring(L, value.data());
    lua_settable(L, -3);
}

void LuaVM::pushtable(int key, string value) {
    lua_pushnumber(L, key);
    lua_pushstring(L, value.data());
    lua_settable(L, -3);
}

void LuaVM::pushtable(int key, int value) {
    lua_pushnumber(L, key);
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}

void LuaVM::pushtable(string key, int value) {
    lua_pushstring(L, key.data());
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}

LuaVM::~LuaVM() {
    lua_close(L);
}

