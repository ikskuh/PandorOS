

local map1 = {
	[72] = { "UP", "UP", "\\0", "\\0", "\\0" },
	[80] = { "DOWN", "DOWN", "\\0", "\\0", "\\0" },
	[75] = { "LEFT", "LEFT", "\\0", "\\0", "\\0" },
	[77] = { "RIGHT", "RIGHT", "\\0", "\\0", "\\0" },
	[83] = { "DELETE", "DELETE", "\\0", "\\0", "\\0" },
}


local f = io.open("lists/keymap-1", "w")

for i=0,255 do
	-- DEFKEY( "7", 7, '7', '/', '{' ),
	local def = map1[i] or { tostring(i), "INVALID", "\\0", "\\0", "\\0" }
	
	f:write(string.format(
		"DEFKEY( \"%s\", %s, '%s', '%s', '%s' ),\n",
		def[1],
		def[2],
		def[3],
		def[4],
		def[5]));
end

f:close();