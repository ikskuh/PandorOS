#include "keyboard-drv.h"
#include "input.h"
#include "io.h"

#include <stddef.h>

#define DEFKEY(name, vkey, lower, upper, variant) { VK_##vkey, lower, upper, variant }

const key_t keymap_set0[] = {
#include "../lists/keymap-0"
};
const key_t keymap_set1[] = {
#include "../lists/keymap-1"
};

struct cpu *keyboard_isr(struct cpu *cpu)
{
	int break_code = 0;

	// Status-Variablen fuer das Behandeln von e0- und e1-Scancodes
	static int      e0_code = 0;
	// Wird auf 1 gesetzt, sobald e1 gelesen wurde, und auf 2, sobald das erste
	// Datenbyte gelesen wurde
	static int      e1_code = 0;
	static uint16_t e1_prev = 0;

	uint8_t kbpInput = inb(0x60);
	struct scancode {
		uint16_t code;
		int16_t set;
	} scancode = {
		0, -1
	};

	// Um einen Breakcode handelt es sich, wenn das oberste Bit gesetzt ist und
	// es kein e0 oder e1 fuer einen Extended-scancode ist
	if ((kbpInput & 0x80) &&
			(e1_code || (kbpInput != 0xE1)) &&
			(e0_code || (kbpInput != 0xE0)))
	{
		break_code = 1;
		kbpInput &= ~0x80;
	}

	if (e0_code) {
		// Fake shift abfangen und ignorieren
		if ((kbpInput == 0x2A) || (kbpInput == 0x36)) {
			e0_code = 0;
			return cpu;
		}
		scancode = (struct scancode){ kbpInput, 1 };
		e0_code = 0;
	} else if (e1_code == 2) {
		// Fertiger e1-Scancode
		// Zweiten Scancode in hoeherwertiges Byte packen
		e1_prev |= ((uint16_t) kbpInput << 8);
		scancode = (struct scancode){ e1_prev, 2 };
		e1_code = 0;
	} else if (e1_code == 1) {
		// Erstes Byte fuer e1-Scancode
		e1_prev = kbpInput;
		e1_code++;
	} else if (kbpInput == 0xE0) {
		// Anfang eines e0-Codes
		e0_code = 1;
	} else if (kbpInput == 0xE1) {
		// Anfang eines e1-Codes
		e1_code = 1;
	} else {
		// Normaler Scancode
		scancode = (struct scancode){ kbpInput, 0 };
	}
	if(scancode.set >= 0) {
		key_t const * map = NULL;
		
		if(scancode.set == 0 && scancode.code < sizeof(keymap_set0) / sizeof(keymap_set0[0])) {
			map = keymap_set0;
		}
		else if(scancode.set == 1 && scancode.code < sizeof(keymap_set1) / sizeof(keymap_set1[0])) {
			map = keymap_set1;
		}
		if(map != NULL) {
			key_t key = map[scancode.code];
			
			keyhit_t event;
			event.scancode.set = scancode.set;
			event.scancode.code = scancode.code;
			event.flags = khfNone;
			event.key = key.code;
			
			if(break_code) {
				event.flags = (keyhitflags_t)(event.flags | khfKeyRelease);
			} else {
				event.flags = (keyhitflags_t)(event.flags | khfKeyPress);
			}
			
			// TODO: Add variant support with alt-graph here...
			if(event.flags & khfKeyPress) {
				if(kbd_is_pressed(VK_SHIFT_LEFT) || kbd_is_pressed(VK_SHIFT_RIGHT)) {
					event.codepoint = key.upper;
				} else {
					event.codepoint = key.lower;
				}
				if(event.codepoint != 0) {
					event.flags = (keyhitflags_t)(event.flags | khfCharInput);
				}
			}
			
			kbd_enqueue_event(event);
		} else {
			// printf("[KBD: %d/%d]", scancode.code, scancode.set);
		}
	}
	
	return cpu;
}
