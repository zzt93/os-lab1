#include "adt/map.h"
#include "kernel/process.h"
#include "drivers/time.h"

extern PCB *current;

MAP(PCB *, int, nb_timers);

/**
   Now every process can only apply for one non-block
   count-down timer at the same time
   repeat application will override the older timer

   Problem:
   - can't tell the difference between the finished timer and
     not adding such timer
     -- fixed by add a gap to timer count
     i.e. range becomes (GAP, GAP + seconds * HZ]
   - can't remove a timer from map, for map not have method to
     delete a pair now.
 */
#define GAP 1

int kstart_non_block_timer(int seconds) {
    if (seconds <= 0) {
        return 0;
    }
    return nb_timers_put(current, seconds * HZ + GAP);
}

int knon_block_timer_finished() {
    return nb_timers_get(current) == GAP;
}

void update_non_block_timer() {
    // only need to update the current running process
    int inter_num = nb_timers_get(current);
    if (inter_num == GAP) {
        // if decrease to 0, not remove or finished will return NULL
        return;
    }
    nb_timers_update(current, inter_num - 1);
}