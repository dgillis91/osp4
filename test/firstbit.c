/* Looking for a way to create process IDs. We
 * will have an array of process control blocks,
 * each PCB having its own process ID. But, we need
 * a way to assign unique IDs. Now, we want to limit
 * the process IDs to the max number, in this case 18.
 * Now, to keep track of the allocated ids, we can use
 * a bitmap. But, for some level of confidence in the
 * size of our bitmap, we can use <stdint.h>.
*/

#include <stdio.h>
#include <stdint.h>

#define PID_BUFFER_LENGTH 18


int main(void) {
    u_int8_t bitmap; 
    u_int32_t i;
    for (i = 0; i < 257; ++i) {
        bitmap = i;
        int first_index_set = __builtin_ffs(~bitmap);
        printf("[+] First set bit index in %d: %d\n", bitmap, first_index_set);
    }
    printf("[+] sizeof(int):               %d\n", (int) sizeof(u_int32_t));
    return 0;
}