#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "../include/proc_table.h"
#include "../include/pclock.h"

#define CLOCK_KEY 8675309
#define PROC_TABLE_KEY 8675310


static int clock_shid;


int main(int argc, char* argv[]) {
    clock_shid = init_clock(CLOCK_KEY);
    if (clock_shid == -1) {
        destruct_clock()
    }

    return EXIT_SUCCESS;
}
