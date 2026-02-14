#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

/* Shell initialization and main loop */
void shell_init(void);
void shell_main_loop(void);
void shell_interactive(void);

/* Command parsing and execution */
typedef void (*command_func)(int argc, char *argv[]);

typedef struct {
    const char *name;
    command_func func;
    const char *help;
} shell_command_t;

/* Built-in commands */
void cmd_help(int argc, char *argv[]);
void cmd_stack(int argc, char *argv[]);
void cmd_gdt(int argc, char *argv[]);
void cmd_halt(int argc, char *argv[]);
void cmd_reboot(int argc, char *argv[]);
void cmd_clear(int argc, char *argv[]);
void cmd_echo(int argc, char *argv[]);
void cmd_about(int argc, char *argv[]);
void cmd_uptime(int argc, char *argv[]);

/* Internal functions */
int shell_parse_input(char *input, char *argv[]);
void shell_execute_command(int argc, char *argv[]);

#endif /* SHELL_H */
