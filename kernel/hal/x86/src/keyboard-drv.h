#pragma once

#include "keyboard.h"
#include "interrupts.h"

/**
 * @brief Key definition on a key map.
 */
typedef struct key
{
	/**
	 * @brief Virtual key code of this scancode.
	 */
	int code;
	
	/**
	 * Lower case representation.
	 */
	codepoint_t lower;
	
	
	/**
	 * Upper case representation.
	 */
	codepoint_t upper;
	 
	 
	/**
	 * Variant (alt+graph) representation.
	 */
	codepoint_t variant;
} key_t;

struct cpu *keyboard_isr(struct cpu *cpu);