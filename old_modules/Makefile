# Lua 5.2.1+ext tcc+nmake
# William Welna <WWelna@OccultusTerra.com>

CFLAGS = -I. -D__MINGW32__ -DLUA_COMPAT_ALL -DLUA_BUILD_AS_DLL -D_WIN32_WINNT=0x0501 -DWIN32 -D__WIN32__ -D_WIN32
LIBS = -L. -llibcurl -lws2_32

OBJECTS = lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o \
          lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o \
          ltm.o lundump.o lvm.o lzio.o \
          lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o \
          lmathlib.o loslib.o lstrlib.o ltablib.o loadlib.o linit.o \
          sqlite3.o lsqlite3.o bit.o \
          auxiliar.o buffer.o except.o inet.o io.o luasocket.o mime.o \
	  options.o select.o tcp.o timeout.o udp.o wsocket.o lfs.o \
	  lfastlz.o fastlz.o lbase32.o Lua-cURL.o Lua-cURL-callback.o \
	  Lua-cURL-getinfo.o Lua-cURL-multi.o Lua-cURL-post.o Lua-cURL-setopt.o \
	  Lua-cURL-share.o Lua-utility.o lzhclib.o lzhxlib.o llzh.o lz4.o lz4hc.o \
	  llz4.o lrandom.o lxxtea.o

LUACOMP = socket.lua ltn12.lua tp.lua url.lua headers.lua\
          mime.lua http.lua ftp.lua smtp.lua \
	  mbox.lua  dumper.lua

default: nyxheader lua521.dll luac.exe lua.exe release

clean:
	rm $(OBJECTS) lua521.dll luac.exe lua.exe lua521.a lua.o luac.o

release:
	@copy /Y lua.exe release
	@copy /Y luac.exe release
	@copy /Y lua521.dll release
	@copy /Y lua521.a release
	@copy /Y lua521.def release

nyxheader:
	@lua buildnumber.lua

lua521.dll: nyx.h $(OBJECTS)
	tcc -shared -o lua521.dll $(OBJECTS) $(LIBS)
lua521.a: nyx.h $(OBJECTS)
	tiny_libmaker lua521.a $(OBJECTS)
luac.exe: nyx.h luac.o lua521.a
	tcc -o luac.exe luac.o lua521.a $(LIBS)
lua.exe: nyx.h lua.o lua521.dll
	tcc -o lua.exe lua.o $(LIBS) -llua521
luaload.bin:
	luac -o luaload.bin $(LUACOMP) 

lapi.o: lapi.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h ltm.h \
 lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h lstring.h ltable.h lundump.h \
 lvm.h
	tcc -c -o lapi.o lapi.c $(CFLAGS)
lauxlib.o: lauxlib.c lua.h luaconf.h lauxlib.h
	tcc -c -o lauxlib.o lauxlib.c $(CFLAGS)
lbaselib.o: lbaselib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o lbaselib.o lbaselib.c $(CFLAGS)
lbitlib.o: lbitlib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o lbitlib.o lbitlib.c $(CFLAGS)
lcode.o: lcode.c lua.h luaconf.h lcode.h llex.h lobject.h llimits.h \
 lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h ldo.h lgc.h \
 lstring.h ltable.h lvm.h
	tcc -c -o lcode.o lcode.c $(CFLAGS)
lcorolib.o: lcorolib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o lcorolib.o lcorolib.c $(CFLAGS)
lctype.o: lctype.c lctype.h lua.h luaconf.h llimits.h
	tcc -c -o lctype.o lctype.c $(CFLAGS)
ldblib.o: ldblib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o ldblib.o ldblib.c $(CFLAGS)
ldebug.o: ldebug.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h lcode.h llex.h lopcodes.h lparser.h ldebug.h ldo.h \
 lfunc.h lstring.h lgc.h ltable.h lvm.h
	tcc -c -o ldebug.o ldebug.c $(CFLAGS)
ldo.o: ldo.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h ltm.h \
 lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h lopcodes.h lparser.h \
 lstring.h ltable.h lundump.h lvm.h
	tcc -c -o ldo.o ldo.c $(CFLAGS)
ldump.o: ldump.c lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h \
 lzio.h lmem.h lundump.h
	tcc -c -o ldump.o ldump.c $(CFLAGS)
lfunc.o: lfunc.c lua.h luaconf.h lfunc.h lobject.h llimits.h lgc.h \
 lstate.h ltm.h lzio.h lmem.h
	tcc -c -o lfunc.o lfunc.c $(CFLAGS)
lgc.o: lgc.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
 lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h
	tcc -c -o lgc.o lgc.c $(CFLAGS)
linit.o: linit.c lua.h luaconf.h lualib.h lauxlib.h
	tcc -c -o linit.o linit.c $(CFLAGS)
liolib.o: liolib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o liolib.o liolib.c $(CFLAGS)
llex.o: llex.c lua.h luaconf.h lctype.h llimits.h ldo.h lobject.h \
 lstate.h ltm.h lzio.h lmem.h llex.h lparser.h lstring.h lgc.h ltable.h
	tcc -c -o llex.o llex.c $(CFLAGS)
lmathlib.o: lmathlib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o lmathlib.o lmathlib.c $(CFLAGS)
lmem.o: lmem.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
 ltm.h lzio.h lmem.h ldo.h lgc.h
	tcc -c -o lmem.o lmem.c $(CFLAGS)
loadlib.o: loadlib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o loadlib.o loadlib.c $(CFLAGS)
lobject.o: lobject.c lua.h luaconf.h lctype.h llimits.h ldebug.h lstate.h \
 lobject.h ltm.h lzio.h lmem.h ldo.h lstring.h lgc.h lvm.h
	tcc -c -o lobject.o lobject.c $(CFLAGS)
lopcodes.o: lopcodes.c lopcodes.h llimits.h lua.h luaconf.h
	tcc -c -o lopcodes.o lopcodes.c $(CFLAGS)
loslib.o: loslib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o loslib.o loslib.c $(CFLAGS)
lparser.o: lparser.c lua.h luaconf.h lcode.h llex.h lobject.h llimits.h \
 lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h ldo.h lfunc.h \
 lstring.h lgc.h ltable.h
	tcc -c -o lparser.o lparser.c $(CFLAGS)
lstate.o: lstate.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h \
 ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h llex.h lstring.h \
 ltable.h
	tcc -c -o lstate.o lstate.c $(CFLAGS)
lstring.o: lstring.c lua.h luaconf.h lmem.h llimits.h lobject.h lstate.h \
 ltm.h lzio.h lstring.h lgc.h
	tcc -c -o lstring.o lstring.c $(CFLAGS)
lstrlib.o: lstrlib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o lstrlib.o lstrlib.c $(CFLAGS)
ltable.o: ltable.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
 ltm.h lzio.h lmem.h ldo.h lgc.h lstring.h ltable.h lvm.h
	tcc -c -o ltable.o ltable.c $(CFLAGS)
ltablib.o: ltablib.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o ltablib.o ltablib.c $(CFLAGS)
ltm.o: ltm.c lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h lzio.h \
 lmem.h lstring.h lgc.h ltable.h
	tcc -c -o ltm.o ltm.c $(CFLAGS)
lua.o: lua.c lua.h luaconf.h lauxlib.h lualib.h
	tcc -c -o lua.o lua.c $(CFLAGS)
luac.o: luac.c lua.h luaconf.h lauxlib.h lobject.h llimits.h lstate.h \
 ltm.h lzio.h lmem.h lundump.h ldebug.h lopcodes.h
	tcc -c -o luac.o luac.c $(CFLAGS)
lundump.o: lundump.c lua.h luaconf.h ldebug.h lstate.h lobject.h \
 llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lstring.h lgc.h lundump.h
	tcc -c -o lundump.o lundump.c $(CFLAGS)
lvm.o: lvm.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
 lzio.h lmem.h ldo.h lfunc.h lgc.h lopcodes.h lstring.h ltable.h lvm.h
	tcc -c -o lvm.o lvm.c $(CFLAGS)
lzio.o: lzio.c lua.h luaconf.h llimits.h lmem.h lstate.h lobject.h ltm.h \
 lzio.h
	tcc -c -o lzio.o lzio.c $(CFLAGS)

lsqlite3.o: lsqlite3.c
	tcc -c -o lsqlite3.o lsqlite3.c $(CFLAGS)
sqlite3.o: sqlite3.c
	tcc -c -o sqlite3.o sqlite3.c -D_WIN32_WINNT=0x0501

bit.o: bit.c
	tcc -c -o bit.o bit.c $(CFLAGS)

fastlz.o: fastlz.c
	tcc -c -o fastlz.o fastlz.c
lfastlz.o: lfastlz.c fastlz.o
	tcc -c -o lfastlz.o lfastlz.c $(CFLAGS)

lz4.o: lz4.c lz4.h
	tcc -c -o lz4.o lz4.c $(CFLAGS)
lz4hc.o: lz4hc.c lz4hc.h
	tcc -c -o lz4hc.o lz4hc.c $(CFLAGS)
llz4.o: llz4.c
	tcc -c -o llz4.o llz4.c $(CFLAGS)

lbase32.o: lbase32.c
	tcc -c -o lbase32.o lbase32.c $(CFLAGS)

Lua-cURL.o: Lua-cURL.c
	tcc -c -o Lua-cURL.o Lua-cURL.c $(CFLAGS)
Lua-cURL-callback.o: Lua-cURL-callback.c
	tcc -c -o Lua-cURL-callback.o Lua-cURL-callback.c $(CFLAGS)
Lua-cURL-getinfo.o: Lua-cURL-getinfo.c
	tcc -c -o Lua-cURL-getinfo.o Lua-cURL-getinfo.c $(CFLAGS)
Lua-cURL-multi.o: Lua-cURL-multi.c
	tcc -c -o Lua-cURL-multi.o Lua-cURL-multi.c $(CFLAGS)
Lua-cURL-post.o: Lua-cURL-post.c
	tcc -c -o Lua-cURL-post.o Lua-cURL-post.c $(CFLAGS)
Lua-cURL-setopt.o: Lua-cURL-setopt.c
	tcc -c -o Lua-cURL-setopt.o Lua-cURL-setopt.c $(CFLAGS)
Lua-cURL-share.o: Lua-cURL-share.c
	tcc -c -o Lua-cURL-share.o Lua-cURL-share.c $(CFLAGS)
Lua-utility.o: Lua-utility.c
	tcc -c -o Lua-utility.o Lua-utility.c $(CFLAGS)

lzhclib.o: lzhclib.c
	tcc -c -o lzhclib.o lzhclib.c $(CFLAGS)
lzhxlib.o: lzhxlib.c
	tcc -c -o lzhxlib.o lzhxlib.c $(CFlAGS)
llzh.o: llzh.c lzhxlib.o lzhclib.o
	tcc -c -o llzh.o llzh.c $(CFLAGS)

lfs.o: lfs.c lfs.h
	tcc -c -o lfs.o lfs.c $(CFLAGS)

lrandom.o: lrandom.c
	tcc -c -o lrandom.o lrandom.c $(CFLAGS)

lxxtea.o: lxxtea.c
	tcc -c -o lxxtea.o lxxtea.c $(CFLAGS)

auxiliar.o: auxiliar.c auxiliar.h buffer.c
	tcc -c -o auxiliar.o auxiliar.c $(CFLAGS)
buffer.o: buffer.c buffer.h io.h timeout.h
	tcc -c -o buffer.o buffer.c $(CFLAGS)
except.o: except.c except.h
	tcc -c -o except.o except.c $(CFLAGS)
inet.o: inet.c inet.h socket.h io.h timeout.h usocket.h
	tcc -c -o inet.o inet.c $(CFLAGS)
io.o: io.c io.h timeout.h
	tcc -c -o io.o io.c $(CFLAGS)
luasocket.o: luasocket.c luasocket.h auxiliar.h except.h \
	timeout.h buffer.h io.h inet.h socket.h usocket.h tcp.h \
	udp.h select.h
	tcc -c -o luasocket.o luasocket.c $(CFLAGS)
mime.o: mime.c mime.h
	tcc -c -o mime.o mime.c $(CFLAGS)
options.o: options.c auxiliar.h options.h socket.h io.h \
	timeout.h usocket.h inet.h
	tcc -c -o options.o options.c $(CFLAGS)
select.o: select.c socket.h io.h timeout.h usocket.h select.h
	tcc -c -o select.o select.c $(CFLAGS)
serial.o: serial.c auxiliar.h socket.h io.h timeout.h usocket.h \
  options.h unix.h buffer.h
	tcc -c -o serial.o serial.c $(CFLAGS)
tcp.o: tcp.c auxiliar.h socket.h io.h timeout.h usocket.h \
	inet.h options.h tcp.h buffer.h
	tcc -c -o tcp.o tcp.c $(CFLAGS)
timeout.o: timeout.c auxiliar.h timeout.h
	tcc -c -o timeout.o timeout.c $(CFLAGS)
udp.o: udp.c auxiliar.h socket.h io.h timeout.h usocket.h \
	inet.h options.h udp.h
	tcc -c -o udp.o udp.c $(CFLAGS)
unix.o: unix.c auxiliar.h socket.h io.h timeout.h usocket.h \
	options.h unix.h buffer.h
	tcc -c -o unix.o unix.c $(CFLAGS)
usocket.o: usocket.c socket.h io.h timeout.h usocket.h
	tcc -c -o usocket.o usocket.c $(CFLAGS)
wsocket.o: wsocket.c socket.h io.h timeout.h usocket.h
	tcc -c -o wsocket.o wsocket.c $(CFLAGS)
