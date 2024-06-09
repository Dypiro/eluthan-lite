#include <stdint.h>
#include <stddef.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include "printf.h"
#include "kernel.h"
#include "keyboard_map.h"
#define pass (void)0
extern uint8_t read_port(uint16_t port);
extern void write_port(uint16_t port, uint8_t value);
void read_keyboard(){
    //write_port(0x60,0xF4);
    /*while (1){
        uint8_t e = read_port(0x60);
        printf("Reading from port ps/2 keyboard (0x60): %u\n", e);
    }*/
    while (1){
    uint8_t e = read_port(0x60);
    if (e > 128){
        pass;
    }
    else{
        char character = keyboard_map[e];
        printf("%c", character);
        e = 129; //make it greater than 128 so that it ignores it
    }
    }
}
void run_command(){
    const char msg[] = "Sup\n";
	printf("%s", msg);
    
}
