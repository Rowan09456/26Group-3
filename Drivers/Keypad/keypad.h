#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

#define KEYPAD_NO_KEY UINT8_C(0xFF)

#ifdef __cplusplus
extern "C" {
#endif

void keypad_init(void);
unsigned char get_keypad_pressedKey(unsigned char keypad_select);

#ifdef __cplusplus
}
#endif

#endif