#pragma once

#include "vkeys.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief A single unicode code point (or character) that stores a text symbol.
 */
typedef uint32_t codepoint_t;

/**
 * @brief Defines different flags for key hits.
 */
typedef enum keyhitflags
{
	khfNone = 0,
	
	/**
	 * The key was pressed.
	 */
	khfKeyPress = (1 << 0),
	
	/**
	 * @brief The key was released
	 */
	khfKeyRelease = (1 << 1),
	
	/**
	 * @brief A character was typed with this event.
	 */
	khfCharInput = (1 << 2),
} keyhitflags_t;

/**
 * @brief A basic keyboard event
 */
typedef struct keyhit
{
	/**
	 * @brief The virtual key that raised the event.
	 */
	int key;
	
	struct {
		int set;
		int code;
	} scancode;
	
	/**
	 * @brief The code point (character) value that is associated with
	 *        the pressed key.
	 */
	codepoint_t codepoint;
	
	/**
	 * @brief Flags that define the specific type and behaviour of this key hit.
	 */
	keyhitflags_t flags;
} keyhit_t;

/**
 * Gets a key event.
 * @param blocking If true, the function will wait for a key to be hit, else the returned key event has flags = 0.
 * @return         A key event that has occurred.
 */
keyhit_t getkey(bool blocking);

/**
 * Blocks until a character is available at the keyboard.
 * @returns The character code that was beeing input.
 */
int getchar();

/**
 * Puts a character into the getchar queue.
 * @param c The character code.
 */
void putchar(int c);

/**
 * Returns if a virtual key is pressed.
 */
bool kbd_is_pressed(int vk);

/**
 * Enqueues a keyhit event.
 */ 
void kbd_enqueue_event(keyhit_t event);