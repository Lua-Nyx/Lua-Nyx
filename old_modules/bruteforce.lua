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

function bruteforce(m, l)
	local self = {}
	local charmap, passmap = {}, {}
	local length, charmap_size = l, m:len()
	local done = false

	function self:step()
		local x
		passmap[1] = passmap[1] + 1;
		for x=1,length do
			if passmap[x] > charmap_size then
				passmap[x] = 1
				if x == length then
					done = true
				else
					passmap[x+1] = passmap[x+1] + 1
				end
			end
			print(">"..x)
		end
	end

	function self:moar()
		self:step()
		if done ~= true then
			return true
		else
			return nil
		end
	end

	function self:next_password()
		local x, ret = 1,""
		for x=1,length do
			ret = ret .. charmap[passmap[x]]
		end
		return ret
	end

	-- Initialize Teh Codes
	local x
	for x=1,charmap_size do
		charmap[x] = string.char(m:byte(x))
	end
	for x=1,length do
		passmap[x] = 1
	end
	x=nil

	return self
end

