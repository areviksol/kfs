#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/* Keyboard port definitions */
#define KB_DATA_PORT    0x60    /* Keyboard data port */
#define KB_STATUS_PORT  0x64    /* Keyboard status port */

/* Keyboard status flags */
#define KB_STATUS_OUTPUT_FULL   0x01    /* Output buffer full */
#define KB_STATUS_INPUT_FULL    0x02    /* Input buffer full */

/* ASCII key definitions */
#define KB_ENTER        0x0D
#define KB_BACKSPACE    0x08
#define KB_TAB          0x09
#define KB_ESCAPE       0x1B

/* Keyboard buffer */
#define KB_BUFFER_SIZE  256
extern volatile uint8_t kb_buffer[KB_BUFFER_SIZE];
extern volatile uint32_t kb_buffer_head;
extern volatile uint32_t kb_buffer_tail;

/* Keyboard initialization and handling */
void keyboard_init(void);
uint8_t keyboard_read_char(void);
void keyboard_wait_for_input(void);
void keyboard_irq_handler(uint8_t scancode);
int keyboard_has_data(void);
uint8_t keyboard_get_char(void);

#endif /* KEYBOARD_H */

