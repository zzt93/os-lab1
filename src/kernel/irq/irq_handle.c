#include "kernel/kernel.h"
#include "kernel/syscall.h"
#include "lib/printk.h"

#define NR_IRQ_HANDLE 32

/* In Nanos, there are no more than 16(actually, 3) hardward interrupts.
   0 -- time interrupt
   1 -- key board interrupt
   14 -- hard disk interrupt
*/
#define NR_HARD_INTR 16

/* Structures below is a linked list of function pointers indicating the
   work to be done for each interrupt. Routines will be called one by
   another when interrupt comes.
   For example, the timer interrupts are used for alarm clocks, so
   device driver of timer will use add_irq_handle to attach a simple
   alarm checker. Also, the IDE driver needs to write back dirty cache
   slots periodically. So the IDE driver also calls add_irq_handle
   to register a handler. */

struct IRQ_t {
    void (*routine)(void);

    struct IRQ_t *next;
};

static struct IRQ_t handle_pool[NR_IRQ_HANDLE];
/**
   prepare a linked list for every irq number
   i.e. for same interrupt, you can register more than one
   handler to handler it.
 */
static struct IRQ_t *handles[NR_HARD_INTR];
static int handle_count = 0;

void
add_irq_handle(int irq, void (*func)(void)) {
    struct IRQ_t *ptr;
    assert(irq < NR_HARD_INTR);
    if (handle_count > NR_IRQ_HANDLE) {
        panic("Too many irq registrations!");
    }
    ptr = &handle_pool[handle_count++]; /* get a free handler */
    ptr->routine = func;
    /*
      if this irq number is already have some handler,
      not to override it but to attach the next.
     */
    ptr->next = handles[irq]; /* insert into the linked list */
    handles[irq] = ptr;
}

void schedule();

extern PCB *current;

void irq_handle(TrapFrame *tf) {
    printk(RED" interrupt "RESET);
    NOINTR;
    assert(current != NULL);
    current->count_of_lock++;
    NOINTR;
    int irq = tf->irq;

    if (irq < 0) {
        panic("Unhandled exception!");
    }

    if (irq == 0x80) {// system call int 0x80
        do_syscall(tf);
    } else if (irq < 1000) {
        printk("Stack: %x %x %x %x; ", tf->eip, tf->ebp, tf->xxx, tf->esp);
        printk("Error code: %x; cr2: %x ", tf->error_code, read_cr2());
        // TODO capture the illegal behaviour of user process and kill it
//        if (current->type == USER) {
//            Msg m;
//            m.buf = current;
//            kexit(&m);
//            prepare_current();
//            return;
//        }

        extern uint8_t logo[];
        panic("Unexpected exception #%d\n\33[1;31mHint: The machine is always right! For more details about exception #%d, see\n%s\n\33[0m",
              irq, irq, logo);
    } else if (irq >= 1000) {
        /* The following code is to handle external interrupts. */
        int irq_id = irq - 1000;
        assert(irq_id < NR_HARD_INTR);
        struct IRQ_t *f = handles[irq_id];
        /* call handlers for this irq one by one */
        while (f != NULL) {
            f->routine();
            NOINTR;
            f = f->next;
        }
    }
    // save the trap frame pointer for the old process
    current->tf = tf;
    schedule();
    // to balance the lock at the beginning of method
    // TODO why shouldn't use `unlock()`
    //unlock();
    NOINTR;
    current->count_of_lock--;
}

