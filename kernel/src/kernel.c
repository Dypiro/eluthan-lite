#include <stdint.h>
#include <stddef.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include "printf.h"
#include "kernel.h"
struct flanterm_context *ft_ctx;
void run_command(){
    const char msg[] = "Sup\n";
    printf("%s", msg);
}
