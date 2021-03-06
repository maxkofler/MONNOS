#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

//////////
//	Debugging options (uncomment to enable)
/*
#define DEBUG_PRINT_SCANCODES
#define DEBUG_PRINT_RELEASES
#define DEBUG_PRINT_SPECIAL_CHAR_CODES
#define DEBUG_PRINT_CAPSLOCK_STATE
#define DEBUG_PRINT_SHIFT_STATE
*/
//
//////////

#include <stdint.h>

#include "kernel/registers.h"

#define BIT_PRESSED			15

extern uint16_t* keymap;
extern uint16_t* keymap_default;
extern uint16_t* keymap_shift;
extern void (*special_char_callback)(uint16_t character);

typedef struct {
	//The state of the key with its modifiers
	uint16_t state;
	
	//The character that was pressed in UPPERCASE
	char character;
} key_t;

enum keyboard_layout{
	LAYOUT_DEFAULT,
	LAYOUT_US,
	LAYOUT_DE,
};

/**
 * @brief	Loads the specified keyboard layout into the driver
 * @param	layout		The new layout to use
 */
void load_keymap(enum keyboard_layout layout);

/**
 * @brief	A handler to process key events, DO NOT CALL THIS OUTSIDE INTERRUPTS
 * @note	This is ment to get called from an interrupt because it reads from the keyboard
 * 			directly
 */
void key_event(registers_t*);

/**
 * @brief	A handler to handle common, non overridden special characters 
 * @param	c			The special character to handle
 */
void common_special_char_callback(uint16_t c);

/**
 * @brief	Processes the supplied keycode (this can be called everywhere)
 * @param	keycode		The keycode to process
 */
void process_keycode(uint8_t keycode);

#define KEYS_SPECIAL	0x0100

//General auxiliary keys
#define KEY_ESCAPE 		0x0101
#define KEY_BACKSPACE 	0x0102
#define KEY_ENTER 		0x0103
#define KEY_SPACE		0x0104
#define KEY_L_CTRL 		0x0105
#define KEY_R_CTRL 		0x0106
#define KEY_L_SHIFT 	0x0107
#define KEY_R_SHIFT		0x0108
#define KEY_L_ALT		0x0109
#define KEY_R_ALT		0x010A
#define KEY_CAPS_LCK	0x010B
#define KEY_SCROLL_LCK	0x010C
#define KEY_NUM_LOCK	0x010D

//Function keys
#define KEY_F1			0x0110
#define KEY_F2			0x0111
#define KEY_F3			0x0112
#define KEY_F4			0x0113
#define KEY_F5			0x0114
#define KEY_F6			0x0115
#define KEY_F7			0x0116
#define KEY_F8			0x0117
#define KEY_F9			0x0118
#define KEY_F10			0x0119
#define KEY_F11			0x011A
#define KEY_F12			0x011B

//Keypad keys
#define KEYP_0			0x0120
#define KEYP_1			0x0121
#define KEYP_2			0x0122
#define KEYP_3			0x0123
#define KEYP_4			0x0124
#define KEYP_5			0x0125
#define KEYP_6			0x0126
#define KEYP_7			0x0127
#define KEYP_8			0x0128
#define KEYP_9			0x0129
#define KEYP_DOT		0x012A
#define KEYP_MINUS		0x012B
#define KEYP_PLUS		0x012C
#define KEYP_TIMES		0x012D
#define KEYP_DIV		0x012E
#define KEYP_ENTER		0x012F

#endif