#include "keyboard.h"

#define KBD_EVENT_QUEUE_SIZE 128

volatile uint32_t kbdEventQueueReadPtr = 0;
volatile uint32_t kbdEventQueueWritePtr = 0;
keyhit_t kbdEventQueue[KBD_EVENT_QUEUE_SIZE];
bool kbdState[VK__LIMIT];

void kbd_enqueue_event(keyhit_t event)
{
	if(event.flags & khfKeyPress) {
		kbdState[(int)event.key] = true;
	} else {
		kbdState[(int)event.key] = false;
	}
	kbdEventQueue[(kbdEventQueueWritePtr++) % KBD_EVENT_QUEUE_SIZE] = event;
	
	if((event.flags & (khfKeyPress | khfKeyRelease)) == 0)
		return;
	// This code will emit key pressed for the "real" virtual keys
	// that have two possible variants to be pressed.
	bool override = true;
	switch(event.key)
	{
		case VK_META_LEFT:
		case VK_META_RIGHT:
			event.key = VK_META;
			break;
		case VK_CONTROL_LEFT:
		case VK_CONTROL_RIGHT:
			event.key = VK_CONTROL;
			break;
		case VK_SHIFT_LEFT:
		case VK_SHIFT_RIGHT:
			event.key = VK_SHIFT;
			break;
		default:
			override = false;
			break;
	}
	if(!override)
		return;
	kbdEventQueue[(kbdEventQueueWritePtr++) % KBD_EVENT_QUEUE_SIZE] = event;
}

bool kbd_is_pressed(int vk)
{
	if(vk < 0 || vk >= VK__LIMIT) {
		switch(vk)
		{
			case VK_CONTROL:
				return kbd_is_pressed(VK_CONTROL_LEFT) || kbd_is_pressed(VK_CONTROL_RIGHT);
			case VK_SHIFT:
				return kbd_is_pressed(VK_SHIFT_LEFT) || kbd_is_pressed(VK_SHIFT_RIGHT);
			case VK_META:
				return kbd_is_pressed(VK_META_LEFT) || kbd_is_pressed(VK_META_RIGHT);
		}
		return false;
	}
	return kbdState[vk];
}

keyhit_t getkey(bool blocking)
{
	if(blocking) {
		while(kbdEventQueueReadPtr >= kbdEventQueueWritePtr); // Burn, baby, burn!
		return kbdEventQueue[(kbdEventQueueReadPtr++) % KBD_EVENT_QUEUE_SIZE];
	} else {
		if(kbdEventQueueReadPtr >= kbdEventQueueWritePtr) {
			return (keyhit_t) { 0, {0, 0}, 0, 0 };
		} else {
			return kbdEventQueue[(kbdEventQueueReadPtr++) % KBD_EVENT_QUEUE_SIZE];
		}
	}
}

// getchar/putchar implementation

int getchar()
{
	keyhit_t key;
	do {
		key = getkey(true);
	} while(!(key.flags & khfCharInput));
	return key.codepoint;
}

void putchar(int c)
{
	kbd_enqueue_event((keyhit_t) {
		VK_INVALID,
		{ 0, 0 },
		c,
		khfCharInput,
	});
}