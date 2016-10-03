#include "keyboard.h"

#define KBD_EVENT_QUEUE_SIZE 128

volatile uint32_t kbdEventQueueReadPtr = 0;
volatile uint32_t kbdEventQueueWritePtr = 0;
keyhit_t kbdEventQueue[KBD_EVENT_QUEUE_SIZE];
bool kbdState[VK__LIMIT];

void kbd_enqueue_event(keyhit_t event)
{
	// TODO: Implement key press/release counter for more precision?
	if(event.flags & khfKeyPress) {
		kbdState[(int)event.key] = true;
	} else {
		kbdState[(int)event.key] = false;
	}
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
			case VK_ALT:
				return kbd_is_pressed(VK_ALT_LEFT) || kbd_is_pressed(VK_ALT_RIGHT);
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