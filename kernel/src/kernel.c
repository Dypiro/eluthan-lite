#include <stdint.h>
#include <stddef.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include "printf.h"
#include "kernel.h"
#include "keyboard.h"
#include "timer.h"
#include "main.h"
#define pass (void)0
int scancode;
int element = 0;
char msg[50] = {};
extern uint8_t read_port(uint16_t port);
extern void write_port(uint16_t port, uint8_t value);
void reset_keyboard(){
    scancode = 129;
    memset(msg,0,sizeof(msg));
    element = 0;
    char msg[50] = {};
}
void keyboard(){
    while (1){
        uint8_t scancode = read_port(0x60);
        if (scancode > 128){
            pass;
            }
        else{
            char character = keyboard_map[scancode];
            printf("%c", character);
            //printf("%u", e);
            if (scancode == 28){
                break;
            }
            else{
                msg[element] = character;
                scancode = 129; //make it greater than 128 so that it ignores it
                delay(400);
                element++;
            }

        }
    }
}
int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return (*str1 == *str2) ? 1 : 0;
}
void kmain(){
    while (1){
        //keyboard manoovering
        printf(">");
        keyboard();
        delay(400);
        if (strcmp(msg, "test")){
            printf("haha yez\n");
        }
        else{
            printf("No such command as: %s\n", msg);
        }
        reset_keyboard();
    }
}