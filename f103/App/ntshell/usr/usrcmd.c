#include "ntopt.h"
#include "ntlibc.h"
#include "ntshell.h"
#include "ntuart.h"
#include "usrcmd.h"
#include "gpio.h"


typedef int (*USRCMDFUNC)(int argc, char **argv);

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int usrcmd_help(int argc, char **argv);
static int usrcmd_led(int argc, char **argv);

typedef struct {
    char *cmd;
    char *desc;
    USRCMDFUNC func;
} cmd_table_t;

static const cmd_table_t cmdlist[] = {
    { "help", "List all commands.", usrcmd_help },
    { "led", "Control the state of LED.", usrcmd_led},
};

int usrcmd_execute(const char *text)
{
    return ntopt_parse(text, usrcmd_ntopt_callback, 0);
}

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    if (argc == 0) {
        return 0;
    }
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        if (ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            return p->func(argc, argv);
        }
        p++;
    }
    uart_puts("Unknown command found.\r\n");
    return 0;
}

static int usrcmd_help(int argc, char **argv)
{
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        uart_puts(p->cmd);
        uart_puts("\t:");
        uart_puts(p->desc);
        uart_puts("\r\n");
        p++;
    }
    return 0;
}

static int usrcmd_led(int argc, char **argv)
{
    if (argc != 2) {
        uart_puts("led on\r\n");
        uart_puts("led off\r\n");
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "on") == 0) {
		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "off") == 0) {
		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
        return 0;
    }
    uart_puts("Unknown sub command found\r\n");
    return -1;
}

int user_callback(const char *text, void *extobj)
{
    usrcmd_execute(text);
    return 0;
} 

ntshell_t nts;
void NTShell_Init(void)
{
	void *extobj = 0;
    uart_puts("NT-Shell.\r\n");
    ntshell_init(&nts, serial_read, serial_write, user_callback, extobj);
    ntshell_set_prompt(&nts, "STM32F1>");
}

void NTShell_Processing(void)
{
	ntshell_execute(&nts);
}

