#include "keyboard.h"

/* Keyboard buffer for IRQ handler */
volatile uint8_t kb_buffer[KB_BUFFER_SIZE];
volatile uint32_t kb_buffer_head = 0;
volatile uint32_t kb_buffer_tail = 0;

/* Keyboard scancode to ASCII conversion table (US layout, lowercase) */
static const char scancode_to_ascii[] = {
    0,    0,    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, 0x09,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0x0D, 0,    'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,    '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,    '*', 0,    ' ',  0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

static const char scancode_to_ascii_shift[] = {
    0,    0,    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0x08, 0x09,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0x0D, 0,    'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,    '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,    '*', 0,    ' ',  0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

/* Keyboard state */
static uint8_t shift_pressed = 0;

/* Port I/O functions */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyboard_init(void) {
    /* Initialize keyboard (8042 controller) */
    shift_pressed = 0;
    kb_buffer_head = 0;
    kb_buffer_tail = 0;
    
    /* Ensure serial port is initialized for reading */
    /* COM1 base address is 0x3F8 */
    /* This is already done in printk.c, but we ensure it's ready */
    /* Serial port should already be initialized by printk() calls */
}

/**
 * IRQ1 Handler - Called from assembly interrupt handler
 * Converts scancode to ASCII and adds to buffer
 */
void keyboard_irq_handler(uint8_t scancode) {
    char ascii = 0;
    
    /* Handle special keys */
    if (scancode == 0x2A) {
        /* Left Shift pressed */
        shift_pressed = 1;
        return;
    }
    if (scancode == 0xAA) {
        /* Left Shift released */
        shift_pressed = 0;
        return;
    }
    
    /* Check if release code (high bit set) */
    if (scancode & 0x80) {
        return;  /* Key release, ignore */
    }
    
    /* Convert scancode to ASCII */
    if (shift_pressed && scancode < 128) {
        ascii = scancode_to_ascii_shift[scancode];
    } else if (scancode < 128) {
        ascii = scancode_to_ascii[scancode];
    }
    
    /* Add to buffer if we have a character */
    if (ascii != 0) {
        uint32_t next_head = (kb_buffer_head + 1) % KB_BUFFER_SIZE;
        if (next_head != kb_buffer_tail) {
            kb_buffer[kb_buffer_head] = ascii;
            kb_buffer_head = next_head;
        }
    }
}

/**
 * Check if there's data in the keyboard buffer
 */
int keyboard_has_data(void) {
    /* First check if there's data in the circular buffer from interrupts */
    if (kb_buffer_head != kb_buffer_tail) {
        return 1;
    }
    
    /* Fallback: poll serial port for input (for QEMU testing with stdio) */
    /* COM1 status port is 0x3FD, bit 0 = data ready */
    uint8_t status = inb(0x3FD);
    if (status & 0x01) {
        /* Data available at serial port */
        uint8_t ch = inb(0x3F8);  /* Read from COM1 data port */
        
        /* Handle special characters from serial input */
        /* QEMU with -serial stdio sends \r (0x0D) for Enter, convert to \n (0x0A) */
        if (ch == '\r') {
            ch = '\n';  /* Convert carriage return to newline */
        }
        
        /* Add to buffer */
        uint32_t next_head = (kb_buffer_head + 1) % KB_BUFFER_SIZE;
        if (next_head != kb_buffer_tail) {
            kb_buffer[kb_buffer_head] = ch;
            kb_buffer_head = next_head;
        }
        return 1;
    }
    
    return 0;
}

/**
 * Get a character from the keyboard buffer
 */
uint8_t keyboard_get_char(void) {
    if (kb_buffer_head == kb_buffer_tail) {
        return 0;  /* No data */
    }
    
    uint8_t ch = kb_buffer[kb_buffer_tail];
    kb_buffer_tail = (kb_buffer_tail + 1) % KB_BUFFER_SIZE;
    return ch;
}

uint8_t keyboard_read_char(void) {
    uint8_t scancode;
    char ascii;
    
    /* Wait for keyboard data */
    keyboard_wait_for_input();
    
    /* Read scancode from keyboard */
    scancode = inb(KB_DATA_PORT);
    
    /* Check for key release (high bit set) */
    if (scancode & 0x80) {
        scancode &= 0x7F;
        
        /* Handle key release */
        if (scancode == 0x36 || scancode == 0x2A) {  /* Left or Right Shift */
            shift_pressed = 0;
        }
        return 0;  /* Key was released, no character */
    }
    
    /* Handle key press */
    if (scancode == 0x36 || scancode == 0x2A) {  /* Left or Right Shift */
        shift_pressed = 1;
        return 0;
    }
    
    /* Special keys */
    if (scancode == 0x0E) {  /* Backspace */
        return KB_BACKSPACE;
    }
    if (scancode == 0x1C) {  /* Enter */
        return KB_ENTER;
    }
    if (scancode == 0x0F) {  /* Tab */
        return KB_TAB;
    }
    if (scancode == 0x01) {  /* Escape */
        return KB_ESCAPE;
    }
    
    /* Convert scancode to ASCII */
    if (scancode < sizeof(scancode_to_ascii)) {
        if (shift_pressed) {
            ascii = scancode_to_ascii_shift[scancode];
        } else {
            ascii = scancode_to_ascii[scancode];
        }
        return ascii;
    }
    
    return 0;  /* Unknown scancode */
}

void keyboard_wait_for_input(void) {
    /* Wait until keyboard data is available */
    while (!(inb(KB_STATUS_PORT) & KB_STATUS_OUTPUT_FULL)) {
        /* Busy wait */
    }
}
