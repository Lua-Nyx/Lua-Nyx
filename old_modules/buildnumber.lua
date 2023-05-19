-- Copyright (C) 2013, William H. Welna All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions are met:
--     * Redistributions of source code must retain the above copyright
--       notice, this list of conditions and the following disclaimer.
--     * Redistributions in binary form must reproduce the above copyright
--       notice, this list of conditions and the following disclaimer in the
--       documentation and/or other materials provided with the distribution.
--
-- THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
-- EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
-- DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
-- DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
-- (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
-- LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
-- ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
-- (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
-- SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-- I Lieks Build Numbers!

nyx_version = "A"
nyx_build = 0;
nyx_date = os.date("%c")

-- Add Build Number
do
	local nofile = io.open("buildnumber.txt", "r")
	local build = nofile:read("*number")
	nofile:close() -- close for reading
	local nofile = io.open("buildnumber.txt", "w") -- reopen for write and erase previous build number
	nyx_build = build +1
	nofile:write(nyx_build)
	nofile:close()
end
-- Add Build Number

header = [[
#ifndef __NYX__H__
#define __NYX__H__

#define LUA_NYX_NOTICE  "LuaNyx %s-%u %s"
#define LUA_NYX_VERSION "%s"
#define LUA_NYX_BUILD "%u"
#define LUA_NYX_DATE "%s"

#endif
]]

buildheader = io.open("nyx.h", "w")
buildheader:write(string.format(header, nyx_version, nyx_build, nyx_date, nyx_version, nyx_build, nyx_date))
buildheader:close()

print("")
print(string.format("LuaNyx %s-%u %s",nyx_version, nyx_build, nyx_date))
print("")
