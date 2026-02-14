#include "shell.h"
#include "printk.h"
#include "lib.h"
#include "keyboard.h"
#include <stdint.h>

/* Port I/O functions */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Shell state */
static uint32_t shell_uptime = 0;
static const char *shell_prompt = "kernel> ";

/* Command buffer */
#define SHELL_INPUT_BUFFER_SIZE 256
static char input_buffer[SHELL_INPUT_BUFFER_SIZE] __attribute__((unused));

/* Argument buffer */
#define SHELL_MAX_ARGS 16
static char *argv[SHELL_MAX_ARGS] __attribute__((unused));

/* Forward declarations for command table */
extern const shell_command_t shell_commands[];
extern const uint32_t shell_commands_count;

/* Built-in command implementations */

void cmd_help(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printk("\n=== Kernel Debugging Shell - Available Commands ===\n\n");
    
    for (uint32_t i = 0; i < shell_commands_count; i++) {
        printk("  %-12s - %s\n", shell_commands[i].name, shell_commands[i].help);
    }
    
    printk("\n");
}

void cmd_stack(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printk("\n");
    print_stack();
}

void cmd_gdt(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printk("\n========== GDT INFORMATION ==========\n");
    printk("GDT Base Address: 0x00000800\n");
    printk("GDT Entry Size: 8 bytes\n");
    printk("Total Descriptors: 7\n\n");
    
    printk("Segment Selectors:\n");
    printk("  Index 0 (0x00): Null Descriptor\n");
    printk("  Index 1 (0x08): Kernel Code Segment (Ring 0)\n");
    printk("  Index 2 (0x10): Kernel Data Segment (Ring 0)\n");
    printk("  Index 3 (0x18): Kernel Stack Segment (Ring 0)\n");
    printk("  Index 4 (0x23): User Code Segment (Ring 3)\n");
    printk("  Index 5 (0x2B): User Data Segment (Ring 3)\n");
    printk("  Index 6 (0x33): User Stack Segment (Ring 3)\n\n");
    
    printk("Memory Layout:\n");
    printk("  Base: 0x00000000 (Flat memory model)\n");
    printk("  Limit: 0xFFFFF (4 GB with page granularity)\n");
    printk("  Granularity: 4 KB pages\n");
    printk("  Default Size: 32-bit\n");
    printk("=====================================\n\n");
}

void cmd_halt(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printk("\n========== SYSTEM HALTING ==========\n");
    
    /* Disable interrupts */
    __asm__ volatile ("cli");
    
    /* Halt CPU */
    __asm__ volatile ("hlt");
    
    /* Should never reach here, but loop just in case */
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void cmd_reboot(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printk("\n========== SYSTEM REBOOTING ==========\n");
    
    /* Disable interrupts */
    __asm__ volatile ("cli");
    
    /* Wait a moment */
    for (volatile int i = 0; i < 1000000; i++) {}
    
    /* Try to reboot via keyboard controller reset */
    uint8_t status;
    
    /* Wait for keyboard controller to be ready */
    do {
        status = inb(0x64);  /* Keyboard status port */
    } while (status & 0x02);
    
    /* Send reset command */
    outb(0x64, 0xFE);
    
    /* If that doesn't work, try triple fault */
    uint16_t idtr[3] = {0, 0, 0};
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("int $3");
    
    /* Fallback: infinite loop */
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void cmd_clear(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    /* Clear VGA screen (for local console) */
    volatile uint16_t *vga = (uint16_t *)0xB8000;
    
    /* Fill entire screen with spaces (80x25) */
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = ' ' | (0x0F << 8);  /* Space with white on black */
    }
    
    /* Reset cursor position to top-left via CRT controller */
    outb(0x3D4, 0x0F);  /* Cursor low byte register */
    outb(0x3D5, 0x00);
    outb(0x3D4, 0x0E);  /* Cursor high byte register */
    outb(0x3D5, 0x00);
    
    /* For serial console, send ANSI clear screen + home cursor */
    printk("\033[2J");  /* ANSI: Clear entire screen */
    printk("\033[H");   /* ANSI: Move cursor to home (0,0) */
}

void cmd_echo(int argc, char *argv[]) {
    if (argc < 2) {
        printk("echo: no arguments\n");
        return;
    }
    
    for (int i = 1; i < argc; i++) {
        printk("%s", argv[i]);
        if (i < argc - 1) printk(" ");
    }
    printk("\n");
}

void cmd_about(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printk("\n========== KERNEL INFORMATION ==========\n");
    printk("Kernel Name: 42 (KFS-2)\n");
    printk("Architecture: i386 (32-bit x86)\n");
    printk("Build Date: February 15, 2026\n");
    printk("Bootloader: GRUB (Multiboot v1)\n");
    printk("Mode: Protected Mode\n");
    printk("Memory: 512 MB (default)\n");
    printk("Kernel Load Address: 0x00100000 (1 MB)\n");
    printk("GDT Address: 0x00000800\n");
    printk("Stack Size: 8 KB\n");
    printk("Features:\n");
    printk("  - Global Descriptor Table (GDT)\n");
    printk("  - Kernel Stack\n");
    printk("  - Printf debugging\n");
    printk("  - Minimalistic Shell\n");
    printk("=========================================\n\n");
}

void cmd_uptime(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printk("Uptime: %d seconds (or cycles)\n\n", shell_uptime);
}

/* Command table */
const shell_command_t shell_commands[] = {
    {"help",   cmd_help,   "Display this help message"},
    {"stack",  cmd_stack,  "Display kernel stack information"},
    {"gdt",    cmd_gdt,    "Display GDT information"},
    {"halt",   cmd_halt,   "Halt the system"},
    {"reboot", cmd_reboot, "Reboot the system (triple fault)"},
    {"clear",  cmd_clear,  "Clear the screen"},
    {"echo",   cmd_echo,   "Echo arguments"},
    {"about",  cmd_about,  "Display kernel information"},
    {"uptime", cmd_uptime, "Display system uptime"},
};

const uint32_t shell_commands_count = sizeof(shell_commands) / sizeof(shell_commands[0]);

/* String utilities for shell */
static int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static char *trim_whitespace(char *str) __attribute__((unused));

static char *trim_whitespace(char *str) {
    /* Skip leading whitespace */
    while (*str && is_whitespace(*str)) {
        str++;
    }
    return str;
}

static int parse_command_line(char *line, char **argv_out, int max_args) __attribute__((unused));

static int parse_command_line(char *line, char **argv_out, int max_args) {
    int argc = 0;
    char *current = trim_whitespace(line);
    
    while (*current && argc < max_args) {
        /* Skip whitespace */
        while (*current && is_whitespace(*current)) {
            current++;
        }
        
        if (!*current) break;
        
        /* Found start of argument */
        argv_out[argc++] = current;
        
        /* Find end of argument */
        while (*current && !is_whitespace(*current)) {
            current++;
        }
        
        /* Null-terminate argument */
        if (*current) {
            *current = '\0';
            current++;
        }
    }
    
    return argc;
}

static void __attribute__((unused)) read_input(char *buffer __attribute__((unused)), 
                                                uint32_t size __attribute__((unused))) {
    /* For now, just use a hardcoded demo */
    /* In a real implementation, you'd read from keyboard interrupt */
    
    /* Simple demo: automatically execute commands for testing */
    /* This will be replaced with actual keyboard input later */
}

/**
 * Parse input string into command and arguments
 */
int shell_parse_input(char *input, char *argv[]) {
    int argc = 0;
    char *ptr = input;
    
    /* Skip leading whitespace */
    while (*ptr && is_whitespace(*ptr)) {
        ptr++;
    }
    
    while (*ptr && argc < SHELL_MAX_ARGS) {
        /* Skip whitespace between arguments */
        while (*ptr && is_whitespace(*ptr)) {
            ptr++;
        }
        
        if (!*ptr) break;
        
        /* Store argument pointer */
        argv[argc++] = ptr;
        
        /* Find end of argument */
        while (*ptr && !is_whitespace(*ptr)) {
            ptr++;
        }
        
        /* Null-terminate argument */
        if (*ptr) {
            *ptr++ = '\0';
        }
    }
    
    return argc;
}

/**
 * Execute a parsed command
 */
void shell_execute_command(int argc, char *argv[]) {
    if (argc == 0) {
        return;  /* Empty command */
    }
    
    const char *cmd = argv[0];
    
    /* Search command table */
    for (int i = 0; shell_commands[i].name != NULL; i++) {
        if (strcmp(cmd, shell_commands[i].name) == 0) {
            shell_commands[i].func(argc, argv);
            return;
        }
    }
    
    /* Command not found */
    printk("Command not found: %s\n", cmd);
    printk("Type 'help' for available commands\n");
}

void shell_init(void) {
    printk("\n");
    printk("╔════════════════════════════════════════╗\n");
    printk("║   Kernel 42 - Debugging Shell v0.1   ║\n");
    printk("║   Type 'help' for available commands  ║\n");
    printk("╚════════════════════════════════════════╝\n");
    printk("\n");
}

void shell_main_loop(void) {
    /* Demo mode: show what the shell can do */
    printk("%s", shell_prompt);
    printk("help\n");
    cmd_help(0, NULL);
    
    printk("%s", shell_prompt);
    printk("about\n");
    cmd_about(0, NULL);
    
    printk("%s", shell_prompt);
    printk("gdt\n");
    cmd_gdt(0, NULL);
    
    printk("%s", shell_prompt);
    printk("stack\n");
    cmd_stack(0, NULL);
    
    printk("%s", shell_prompt);
    printk("echo Hello from the kernel shell!\n");
    {
        char *demo_argv[] = {"echo", "Hello", "from", "the", "kernel", "shell!"};
        cmd_echo(6, demo_argv);
    }
    
    printk("%s", shell_prompt);
    printk("uptime\n");
    cmd_uptime(0, NULL);
    
    printk("\n");
    
    /* Interactive shell - read from keyboard */
    shell_interactive();
}

/**
 * Interactive shell - reads commands from keyboard
 */
void shell_interactive(void) {
    char cmd_input_buffer[256];
    uint32_t input_pos = 0;
    char *cmd_argv[SHELL_MAX_ARGS];
    uint32_t idle_count = 0;
    
    printk("%s", shell_prompt);
    
    while (1) {
        if (keyboard_has_data()) {
            uint8_t ch = keyboard_get_char();
            idle_count = 0;
            
            if (ch == 0x0A || ch == 0x0D) {
                /* Enter/newline key pressed */
                cmd_input_buffer[input_pos] = '\0';
                printk("\n");
                
                if (input_pos > 0) {
                    /* Parse and execute command */
                    int argc = shell_parse_input(cmd_input_buffer, cmd_argv);
                    shell_execute_command(argc, cmd_argv);
                }
                
                input_pos = 0;
                printk("%s", shell_prompt);
                
            } else if (ch == 0x08 || ch == 0x7F) {
                /* Backspace or Delete */
                if (input_pos > 0) {
                    input_pos--;
                    printk("\b \b");  /* Backspace, space, backspace */
                }
                
            } else if (ch >= 32 && ch < 127) {
                /* Printable character */
                if (input_pos < 255) {
                    cmd_input_buffer[input_pos++] = ch;
                    printk("%c", ch);
                }
                
            } else if (ch == 0x09) {
                /* Tab - just print spaces */
                if (input_pos < 255) {
                    cmd_input_buffer[input_pos++] = ' ';
                    printk("    ");
                }
            }
            /* Ignore other control characters */
        } else {
            /* Idle - small busy-wait to avoid excessive CPU */
            idle_count++;
            if (idle_count > 10000) {
                idle_count = 0;
                __asm__ volatile("pause");  /* x86 pause instruction */
            }
        }
    }
}
