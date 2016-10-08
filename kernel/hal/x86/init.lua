

local map1 = {
	[29] = { "CONTROL_LEFT", "CONTROL_LEFT", "\\0", "\\0", "\\0" },
	[53] = { "NUM_DIV", "NUM_DIV", "\\0", "\\0", "\\0" },
	[55] = { "PRINT", "PRINT", "\\0", "\\0", "\\0" },
	[56] = { "ALT_GRAPH", "ALT_GRAPH", "\\0", "\\0", "\\0" },
	[71] = { "HOME", "HOME", "\\0", "\\0", "\\0" },
	[72] = { "UP", "UP", "\\0", "\\0", "\\0" },
	[73] = { "PAGE_UP", "PAGE_UP", "\\0", "\\0", "\\0" },
	[75] = { "LEFT", "LEFT", "\\0", "\\0", "\\0" },
	[77] = { "RIGHT", "RIGHT", "\\0", "\\0", "\\0" },
	[79] = { "END", "END", "\\0", "\\0", "\\0" },
	[80] = { "DOWN", "DOWN", "\\0", "\\0", "\\0" },
	[81] = { "PAGE_DOWN", "PAGE_DOWN", "\\0", "\\0", "\\0" },
	[82] = { "INSERT", "INSERT", "\\0", "\\0", "\\0" },
	[83] = { "DELETE", "DELETE", "\\0", "\\0", "\\0" },
	[91] = { "META", "META", "\\0", "\\0", "\\0" },
	[93] = { "MENU", "MENU", "\\0", "\\0", "\\0" },
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