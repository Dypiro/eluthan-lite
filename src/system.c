#include "system.h"
#include <stddef.h>
// Video memory is located at 0xb8000
char *vidptr = (char*)0xb8000;
int index = 0;
char attr = 0x07;
int term_ind = 10;
char command[80*25];
int cmd_ind = 0;
int is_cmd = 0;

int pt_s = 0;
int is_shift = 0;
int strt = 1;

int reg[26];
void outb8(uint16_t port, uint8_t value) {
    asm("outb %1, %0" : : "dN" (port), "a" (value));
}
uint8_t inb8(uint16_t port) {
    uint8_t r;
    asm("inb %1, %0" : "=a" (r) : "dN" (port));
    return r;
}
// Methods for resetting after a new line
//(and restricting reset if needed)
void clear_command() {
	int i = 0;
	while(i < 80*25) {
		command[i] = '\0';
		i++;
	}
	
	cmd_ind = 0;
}
void is_cmd_on() {
	is_cmd = 1;
}
void is_cmd_off() {
	is_cmd = 0;
}
void reboot() {// Its just a 8042 reset copied from osdev.org
  uint8_t good = 0x02;
  while (good & 0x02)
    good = inb8(0x64);
  outb8(0x64, 0xFE);
  asm("hlt");
}
// random size_t generator
size_t seed = 1;
const size_t a = 2001;
const size_t c = 1 << 30;
const size_t m = (1 << 63) - 1;

size_t random(size_t min, size_t max) {
	size_t range = max - min;
	seed = (a * seed + c) % m;
	return min + (size_t)(seed % range);
}
// Main function for executing terminal commands
// All commands with arguments require 1 space in between opcode and argument

// size_t to string converter
void size_tToCharArray(size_t number, char* charArray) {
    int i = 0;

    // Handle the case of zero
    if (number == 0) {
        charArray[0] = '0';
        charArray[1] = '\0';
        return;
    }

    // Convert each digit of the size_t to a character
    while (number > 0) {
        charArray[i++] = (char)((number % 10) + '0');
        number /= 10;
    }

    charArray[i] = '\0';

    // Reverse the charArray so that it's in the correct order
    int length = i;
    for (i = 0; i < length / 2; i++) {
        char temp = charArray[i];
        charArray[i] = charArray[length - i - 1];
        charArray[length - i - 1] = temp;
    }
}

void run_command() {
	is_cmd = 1;
	int size = str_len(command);
	
	// halts the cpu
	if(str_startswith(command, "reboot") == 1) {
		reboot();
	}
	/* prints out string next to it with a new line
	 * EX:
	 * println Hello, world!!
	 * OUTPUT:
	 * 	Hello, world!!
	 *
	 */
	else if(str_startswith(command, "crash")) {
		char a = 'a';
		char b = 'b';
		if(letti(a)!=-1 && letti(b)!=-1)
			reg[letti(a)] /= reg[letti(b)];
	}
	else if(str_startswith(command, "println")) {
		putslns(command, 8, size);
	} 
	/* prints the value of the variable next to command and adds a newline
	 * EX:
	 * assume register a is equal to "Hello, world!!"
	 * printvln a
	 * OUTPUT:
	 * Hello, world!!
	 *
	 */
	else if(str_startswith(command, "printvln")) {
		if(letti(command[9]) != -1)
			putnumln(reg[letti(command[9])]);
	/* prints the value of the variable next to command
	 * EX:
	 * assume register a is equal to "Hello, world!!"
	 * printv a
	 * OUTPUT:
	 * Hello, world!!
	 */
	} else if(str_startswith(command, "printv")) {
		if(letti(command[7]) != -1)
			putnum(reg[letti(command[7])]);
	/* prints the value of the string next to command
	 * EX:
	 * print Hello, world!!
	 * OUTPUT:
	 * Hello, world!!
	 */
	} else if(str_startswith(command, "print")) {
		putss(command, 6, size);
	// Clears all previous text from the screen
	} else if(str_startswith(command, "rng")){
		size_t rand = random(0, 100);
		char rand_str[4];
		size_tToCharArray(rand, rand_str);
		putsln(rand_str);
	} else if (str_startswith(command, "clear")) {
		clear_screen();
		index = 0;
	/* Sets variable after command to value after var name
	 * EX:
	 * setv a Hello, world!!
	 * WHAT IT DOES:
	 * variable a now has the value of "Hello, world!!";
	 */
    } else if(str_startswith(command, "setv")) {
		if(letti(command[5]) != -1) {
			int r = letti(command[5]);
			
			int i = 0;
			while(i < 7) {
				command[i] = ' ';
				i++;
			}
			
			reg[r] = atoi(command);
		}
	} else if(str_startswith(command, "cc") == 1) {
		if(str_startswith(command, "cc help")) {
			putsln("Colors:");
			putsln(">>> BLACK       :: 0");
			putsln(">>> BLUE        :: 1");
			putsln(">>> GREEN       :: 2");
			putsln(">>> CYAN        :: 3");
			putsln(">>> RED         :: 4");
			putsln(">>> MAGENTA     :: 5");
			putsln(">>> BROWN       :: 6");
			putsln(">>> LT GRAY     :: 7");
			putsln(">>> DK GRAY     :: 8");
			putsln(">>> LT BLUE     :: 9");
			putsln(">>> LT GREEN    :: A");
			putsln(">>> LT CYAN     :: B");
			putsln(">>> LT RED      :: C");
			putsln(">>> LT MAGENTA  :: D");
			putsln(">>> LT BROWN    :: E");
			putsln(">>> WHITE       :: F");
		} else {
			char b = command[4];
			char f = command[3];
			if(f > '@' && b > '@')
				text_color(f - '0', b - 55);
			else if(f > '@')
				text_color(f - 55, b - '0');
			else if(b > '@')
				text_color(f - '0', b - 55);
			else
				text_color(f - '0', b - '0');
		}
	} else if(str_startswith(command, "help") == 1) {
		putsln("Commands:");
		putsln(">>> reboot       :: reboots the system back to GRUB");
		putsln(">>> cc [color]   :: change text color - (fore, back)");
		putsln(">>> cc help      :: displays on how to change the text color");
		putsln(">>> print text   :: print out a piece of text");
		putsln(">>> printv let   :: print out the value of variable let");
		putsln(">>> printvln let :: print out the value of variable let with a line after");
		putsln(">>> setv let val :: set variable let to val");
		putsln(">>> clear        :: clears the screen");
		putsln(">>> rng          :: displays a random number");
		putsln(">>> println text :: print out a piece of text with a line");
		putsln(">>> help         :: show help command");
		putsln(">>> crash        :: Crashes the pc");
	} else {
		puts(">>> Unknown command: ");
		putsln(command);
		putsln(">>> Try help.");
	}
	clear_command();
	
	is_cmd = 0;
}

// Convert a string to an int;
int atoi(char *str) {
	int res = 0; // result
	int i = 0;
	
	int is_negative = 1;
	
	while(str[i] != '\0') {
		if(str[i] - '0' < 10 && str[i] - '0' >= 0) {
			res = res*10 + str[i] - '0';
		} else if(str[i] == '-') {
			is_negative = -1;
		} else {
			// Error handling for if it doesn't work
			putsln("Error: String parsing - Value will be 0!");
			res = 0;
			break;
		}
		i++;
	}
	return res*is_negative;
}

// Converts register input (letter) into index value (reg)
int letti(char let) {
	switch(let) {
		case 'a':
			return 0;
		break;
		case 'b':
			return 1;
		break;
		case 'c':
			return 2;
		break;
		case 'd':
			return 3;
		break;
		case 'e':
			return 4;
		break;
		case 'f':
			return 5;
		break;
		case 'g':
			return 6;
		break;
		case 'h':
			return 7;
		break;
		case 'i':
			return 8;
		break;
		case 'j':
			return 9;
		break;
		case 'k':
			return 10;
		break;
		case 'l':
			return 11;
		break;
		case 'm':
			return 12;
		break;
		case 'n':
			return 13;
		break;
		case 'o':
			return 14;
		break;
		case 'p':
			return 15;
		break;
		case 'q':
			return 16;
		break;
		case 'r':
			return 17;
		break;
		case 's':
			return 18;
		break;
		case 't':
			return 19;
		break;
		case 'u':
			return 20;
		break;
		case 'v':
			return 21;
		break;
		case 'w':
			return 22;
		break;
		case 'x':
			return 23;
		break;
		case 'y':
			return 24;
		break;
		case 'z':
			return 25;
		break;
	}
	
	// Adds error message if not lower case letter
	putsln("Error: Unknown register - Nothing will be set!!");
	return -1;
}

// Prints the value of a number
void putnum(int num) {
	int res = num;
	
    if (res==0) 
		putch('0');
    else {
        if (res<0) { 
			putch('-');
			res=-res; 
		}
		
        /*Largest num is `2,147,483,64(7|8)`*/
        char temp[10];
        int i = 0;
        do {
			if(i >= 9) {
				// If number exceeds 10 digits
				putsln("Error: Number too large - will only be 10 digits");
				break;
			}
			
            temp[i++] = res%10 + '0';
            res /= 10;
        } while(res);
		
        while (--i>=0) {
            putch(temp[i]);
        }
    }
}

// Prints the value of a number and an end line
void putnumln(int num) {
	putnum(num);
	
	putsln("");
}

// Gets the attribute (color and back) at video memory address
char get_attr(int ind) {
	return vidptr[ind*2+1];
}

// returns the length of a string
int str_len(char *str) {
	int i = 0;
	while(str[i] != '\0') {
		i++;
	}
	return i;
}

// initializes the terminal
void set_up_terminal() {
	char c = attr;
	text_color(WHITE, BLACK);
	puts(">");
	term_ind = index;
	clear_command();
	attr = c;
}

// Prints a single character to the screen at the next place (scrolls)
void putch(char c) {
	scroll(c);
	
	if(c == '\n') {
		is_shift = 0;
		int i = index/2;
		i += 80;
		while(i % 80 > 0) {
			i--;
		}
		
		index = i*2;
		move_cursor((index/2)%80, (index/2)/80);
		
		if(strt == 0)
			run_command();
		
		char r = attr;
		
		if(strt == 0) {
			text_color(WHITE, BLACK);
			is_cmd = 1;
			puts(">");
			is_cmd = 0;
			term_ind = index;
		} else {
			strt = 0;
		}
		
		attr = r;
	} if(c == '\r') {
		if(is_shift == 0)
			is_shift = 1;
		else if(is_shift == 1)
			is_shift = 0;
	} else {
		if(c == '\t') {
			putch(' ');
			putch(' ');
			putch(' ');
			putch(' ');
		} else if(c == '\b') {
			if(index > term_ind) {
				index -= 2;
				vidptr[index] = ' ';
				vidptr[index + 1] = attr;
				
				if(is_cmd == 0) {
					cmd_ind--;
					command[cmd_ind] = '\0';
				}
				
				move_cursor((index/2)%80, (index/2)/80);
			}
		} else if(c >= ' '){
			if(is_shift == 1 && strt == 0) {
				if(c > '`')
					c = c - 32;
				else
					c = c - 16;
			}
			vidptr[index] = c;
			vidptr[index+1] = attr;
			
			if(is_cmd == 0) {
				command[cmd_ind] = c;
				cmd_ind++;
			}
			
			index += 2;
			
			move_cursor((index/2)%80, (index/2)/80);
		}
	}
}

// Returns the character at a position in memory
char getch(int ind) {
	return vidptr[ind];
}

// sets the color at a position in video memory
void text_color(unsigned char forecolor, unsigned char backcolor) {
	attr = (backcolor << 4) | (forecolor & 0x0F);
}

// Determines if a string starts with another string
int str_startswith(char *one, char *two) {
	int ol = str_len(one);
	int tl = str_len(two);
	
	if(tl > ol) {
		return 0;
	} else {
		int i = 0;
		while(i < tl) {
			if(one[i] != two[i])
				return 0;
			i++;
		}
		
		return 1;
	}
}

// Clears out the video memory
void clear_screen(void)
{
	int i = 0;
	while (i < 80*25*2) {
		vidptr[i] = ' ';
		i++;
		vidptr[i] = attr;
		i++;
	}
}

// Writes out a string to the screen
void puts(char *str) {
	pt_s = 1;
	int length = str_len(str);
	int i =0;
	while(i < length) {
		putch(str[i]);
		i++;
	}
	pt_s = 0;
}

// Writes out a string to the screen with a new line
void putsln(char *str) {
	pt_s = 1;
	int length = str_len(str);
	int i =0;
	while(i < length) {
		putch(str[i]);
		i++;
	}
	
	strt = 1;
	putch('\n');
	pt_s = 0;
}

// Print out a substring
void putss(char *str, int beg, int end) {
	pt_s = 1;
	int i = beg;
	while(i < end) {
		putch(str[i]);
		i++;
	}
	pt_s = 0;
}

// Print out a substring and a line
void putslns(char *str, int beg, int end) {
	pt_s = 1;
	int i = beg;
	while(i < end) {
		putch(str[i]);
		i++;
	}
	
	strt = 1;
	putch('\n');
	pt_s = 0;
}

// Sets the position of the next character
void move_cursor(int col, int row) {
	unsigned short position=(row*80) + col;
	
	// cursor LOW port to vga INDEX register
	write_port(0x3D4, 0x0F);
	write_port(0x3D5, (unsigned char)(position&0xFF));
	// cursor HIGH port to vga INDEX register
	write_port(0x3D4, 0x0E);
	write_port(0x3D5, (unsigned char )((position>>8)&0xFF));
 }

/* Complex code for scrolling line
 * Deletes top line
 * Moves every character back to the ength of that line (start of second line becomes start of first line)
 * Clears bottom line
 * Moves cursor to the start of the bottom line
 */
void scroll(char c)
{
    if(index >= 80*25*2) {
		index = 0;
		while(index < 80*24*2) {
			vidptr[index] = vidptr[index + 160];
			vidptr[index + 1] = vidptr[index + 161];
			index += 2;
		}
		
		index = 80*24*2;
		
		while(index < 80*25*2) {
			putch(' ');
		}
		
		index = 80*24*2;
		
		if(pt_s == 0)
			putch(c);
		
		move_cursor((index/2)%80, (index/2)/80);
	}
}

// Basic functions for memory manipulation
void *memcpy(void *dest, const void *src, int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}
void *memset(void *dest, char val, int count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}
unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}