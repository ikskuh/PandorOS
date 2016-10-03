
if not arg[1] then
	error("PLATFORM argument missing.")
end

virtual_keys = {
	"INVALID",
	"ESCAPE",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
	"LEFT", "RIGHT", "UP", "DOWN",
	"BACKSPACE", "DELETE", 
	"TAB",
	"PLUS",
	"RETURN",
	"CONTROL_LEFT",
	"CONTROL_RIGHT",
	"SHIFT_LEFT",
	"SHIFT_RIGHT",
	"ALT_LEFT",
	"ALT_RIGHT",
	"CAPS_LOCK",
	"SCROLL_LOCK",
	"NUM_LOCK",
	"SPACE",
	"COMMA",
	"DOT",
	"HYPHEN",
	"NUMBER", -- #
	"NUM_PLUS",
	"NUM_MINUS",
	"NUM_MULT",
	"NUM_DOT",
	"NUM_0", "NUM_1", "NUM_2", "NUM_3", "NUM_4", "NUM_5", "NUM_6", "NUM_7", "NUM_8", "NUM_9",
	"_LIMIT"
	-- "ü", "ö", "ä", "^"
}


local f = io.open("include/vkeys.h", "w")

f:write("#pragma once", "\n")
for i,v in pairs(virtual_keys) do
	f:write("#define VK_", v, " ", i - 1, "\n")
end
f:close();
