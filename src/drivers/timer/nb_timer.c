#include "adt/map.h"
#include "kernel/process.h"
#include "drivers/time.h"

extern PCB *current;

MAP(PCB *, int, nb_timers);

/**
   Now every process can only apply for one non-block
   count-down timer at the same time
   repeat application will override the older timer
 */
int kstart_non_block_timer(int seconds) {
    if (seconds < 0) {
        return 0;
    }
    return nb_timers_put(current, seconds * HZ);
}

int knon_block_timer_finished() {
    return nb_timers_get(current) == 0;
}

void update_non_block_timer() {
    // only need to update the current running process
    int inter_num = nb_timers_get(current);
    if (inter_num == 0) {
        return;
    }
    nb_timers_update(current, inter_num - 1);
}