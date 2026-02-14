# Kernel Debugging Shell

## Overview

A minimalistic shell has been implemented for kernel debugging and testing. The shell provides a command-line interface with several useful debugging commands.

## Features

- **Command-based interface** - Simple command parsing and execution
- **Built-in commands** - Stack display, GDT info, system control
- **Help system** - Built-in help for all commands
- **Command history ready** - Framework for future keyboard input
- **Debugging utilities** - Stack inspection, memory information

## Built-in Commands

### help
Display all available commands and their descriptions.

```
kernel> help
=== Kernel Debugging Shell - Available Commands ===

  help        - Display this help message
  stack       - Display kernel stack information
  gdt         - Display GDT information
  halt        - Halt the system
  reboot      - Reboot the system (triple fault)
  clear       - Clear the screen
  echo        - Echo arguments
  about       - Display kernel information
  uptime      - Display system uptime
```

### stack
Display kernel stack information including ESP, EBP, and first 16 stack entries.

```
kernel> stack
========== KERNEL STACK INFO ==========
Stack Pointer (ESP): 0x00101800
Base Pointer (EBP): 0x00101800
...
```

### gdt
Display Global Descriptor Table information including all descriptors and memory layout.

```
kernel> gdt
========== GDT INFORMATION ==========
GDT Base Address: 0x00000800
GDT Entry Size: 8 bytes
Total Descriptors: 7

Segment Selectors:
  Index 0 (0x00): Null Descriptor
  Index 1 (0x08): Kernel Code Segment (Ring 0)
  ...
```

### about
Display kernel and system information.

```
kernel> about
========== KERNEL INFORMATION ==========
Kernel Name: 42 (KFS-2)
Architecture: i386 (32-bit x86)
Build Date: February 15, 2026
...
```

### clear
Clear the VGA screen and reset cursor position.

```
kernel> clear
[Screen cleared]
```

### echo
Echo arguments to the console.

```
kernel> echo Hello World
Hello World
```

### uptime
Display system uptime (in seconds or cycles).

```
kernel> uptime
Uptime: 0 seconds (or cycles)
```

### halt
Halt the system (disable interrupts and execute HLT instruction).

```
kernel> halt
System halting...
[CPU halted]
```

### reboot
Reboot the system using triple fault method.

```
kernel> reboot
System rebooting...
[System restarts]
```

## Files

### New Files

- **shell.h** - Shell interface and command definitions
- **shell.c** - Shell implementation with built-in commands
- **keyboard.h** - Keyboard input interface
- **keyboard.c** - PS/2 keyboard driver (scancode to ASCII conversion)

### Modified Files

- **kernel.c** - Calls shell_init() and shell_main_loop()

## Shell Architecture

### Command Structure

Each command is a function following this signature:

```c
void cmd_name(int argc, char *argv[])
```

Commands are registered in a command table:

```c
typedef struct {
    const char *name;
    command_func func;
    const char *help;
} shell_command_t;
```

### Command Parsing

The shell parses command lines by:
1. Trimming leading/trailing whitespace
2. Splitting on whitespace to create arguments
3. Looking up the command in the command table
4. Executing the corresponding function

### Demo Mode

Currently, the shell runs in demo mode, automatically executing sample commands to showcase functionality:

```
kernel> help
[displays help]

kernel> about
[displays kernel info]

kernel> gdt
[displays GDT info]

kernel> stack
[displays stack info]

kernel> echo Hello from the kernel shell!
Hello from the kernel shell!

kernel> uptime
Uptime: 0 seconds (or cycles)

kernel> 
[Waiting for keyboard input]
```

## Keyboard Input (Framework)

The keyboard module provides:

- **Scancode to ASCII conversion** - Full US keyboard layout
- **Shift key support** - Uppercase letters and symbols
- **Special key handling** - Enter, Backspace, Tab, Escape
- **Modular design** - Ready for PS/2 controller integration

Keyboard functions available:

```c
void keyboard_init(void);          // Initialize keyboard
uint8_t keyboard_read_char(void);  // Read next character
void keyboard_wait_for_input(void);// Wait for data
```

## Current Limitations

1. **No real keyboard input** - Demo mode shows commands being executed
2. **No interrupt handling** - Would need IDT implementation
3. **No command history** - Framework ready for implementation
4. **No autocomplete** - Can be added later
5. **No pipes or redirects** - Beyond scope of minimalistic shell

## Future Enhancements

1. **Keyboard input integration** - Connect PS/2 keyboard driver
2. **Command history** - Store previous commands
3. **Autocomplete** - Tab-complete commands
4. **Aliases** - Short names for common commands
5. **System statistics** - CPU info, memory usage, etc.
6. **Memory viewer** - peek/poke for memory inspection
7. **Register display** - Show CPU register values
8. **Breakpoint support** - For debugging (requires IDT)

## Example Usage

### Display Help

```
kernel> help
=== Kernel Debugging Shell - Available Commands ===

  help        - Display this help message
  stack       - Display kernel stack information
  gdt         - Display GDT information
  ...
```

### Inspect Stack

```
kernel> stack
========== KERNEL STACK INFO ==========
Stack Pointer (ESP): 0x00101800
Base Pointer (EBP): 0x00101800

Stack Contents (first 16 entries):
[00101800] = 0x2BADB002
[00101804] = 0x00010000
...
```

### Get System Info

```
kernel> about
========== KERNEL INFORMATION ==========
Kernel Name: 42 (KFS-2)
Architecture: i386 (32-bit x86)
Build Date: February 15, 2026
Bootloader: GRUB (Multiboot v1)
Mode: Protected Mode
...
```

## Implementation Notes

### Thread-Safe Design
- No global state modifications during command execution
- Commands are pure functions (mostly)
- No locking required (single-threaded kernel)

### Memory Efficiency
- Fixed-size buffers (256 bytes for input)
- Maximum 16 arguments per command
- Minimal dynamic allocation

### Extensibility
- Easy to add new commands
- Simple command table registration
- Consistent interface for all commands

## Integration Points

### With GDT
- Access to GDT information via `gdt` command
- Display of segment selectors and descriptors

### With Stack
- Integration with existing print_stack() function
- Full stack display via `stack` command

### With Kernel
- Called from kmain() after GDT initialization
- Runs in kernel context with full privileges

### With Keyboard (Future)
- Scancode to ASCII conversion ready
- Framework for interrupt-driven input
- Awaiting IDT implementation

## Command Table

| Command | Function | Purpose |
|---------|----------|---------|
| help | cmd_help | Display help |
| stack | cmd_stack | Show stack info |
| gdt | cmd_gdt | Show GDT details |
| halt | cmd_halt | Halt CPU |
| reboot | cmd_reboot | Restart system |
| clear | cmd_clear | Clear screen |
| echo | cmd_echo | Echo text |
| about | cmd_about | Show kernel info |
| uptime | cmd_uptime | Show uptime |

## Code Organization

```
shell.h
├─ shell_command_t structure
├─ Function declarations
└─ Constants

shell.c
├─ Command implementations
├─ Command table
├─ Parsing logic
└─ Shell main loop

keyboard.h
├─ Keyboard constants
└─ Function declarations

keyboard.c
├─ Scancode tables
├─ Keyboard state
└─ Character conversion
```

## Building with Shell

```bash
make clean
make all      # Includes shell.c and keyboard.c
make iso
make run
```

The shell will automatically start after GDT initialization and display a demo of available commands.
