#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/semaphore.h"
#include "lib/string.h"
#include "lib/malloc.h"


/**
   Intenal:
   send() message to A will copy to A's list
   A receive() will copy to it's parameter

   Message sender:
   If this method can be invoked by different thread
   simultaneously, the message it used to send is either
   a `local variable` or `method parameter` -- for the
   implementation, they are always right.
   If this method is single-threaded, so use `static` is fine
   Message receiver:
   single-threaded -- can use static
   multi-threaded -- must local variable or parameter to reply
 */

void add_message(PCB* p, Msg* msg) {
    NOINTR;
    Msg* m = kmalloc(sizeof(Msg));
    memcpy(m, msg, sizeof(Msg));
    list_add_after(&(p->mes), &(m->list));
}

static int has_message(PCB* p, pid_t id) {
    ListHead* head = &(p->mes);
    ListHead* ptr = NULL;
    Msg* tmp = NULL;
    NOINTR;
    list_foreach(ptr, head) {
        assert(ptr != NULL);
        tmp = list_entry(ptr, Msg, list);
        assert(tmp != NULL);
        if (tmp->src == id || id == ANY) {
            break;
        }
    }
    return ptr != head;
}

static Msg* find_message(PCB* p, pid_t id) {
    ListHead* head = &(p->mes);
    ListHead* ptr = NULL;
    Msg* tmp = NULL;
    list_foreach(ptr, head) {
        assert(ptr != NULL);
        tmp = list_entry(ptr, Msg, list);
        NOINTR;
        assert(tmp != NULL);
        if (tmp->src == id || id == ANY) {
            list_del(&(tmp->list));
            break;
        }
    }
    assert(ptr != head);
    return tmp;
}
/**
   get a message from process p's message list, with the right
   id, then copy to m.
 */
void get_message(PCB* p, pid_t id, Msg* m) {
    // find and delete aim message
    Msg *tmp = find_message(p, id);
    printk("#%d get_message: src %d, dest %d\n", current->pid, tmp->src, tmp->dest);
    assert(tmp != NULL
        && (tmp->src == id || id == ANY)
        && tmp->dest == current->pid);
    // copy to m
    memcpy(m, tmp, sizeof(Msg));
    kfree(tmp);
}

/**
   can't use P() & V() in send
   and message for P() and V() can
   happen one after another, may cause deadlocks
 */
void send(pid_t dest, Msg *m) {
    //printk("%d:----send to %d------\n", current->pid, dest);
    lock();
    NOINTR;
    PCB* de = fetch_pcb(dest);
    NOINTR;
    printk("sd %d:-------#%d send to #%d------\n", current->pid, m->src, dest);
    assert(de != NULL && dest == de->pid);
    NOINTR;
    //Sem* s = &(de->mes_lock);
    //P(s);
    //m->src = current->pid;
    m->dest = dest;
    add_message(de, m);
    //V(s);
    wake_up(de);
    NOINTR;
    unlock();
    //printk("%d:---------end send--------\n", current->pid);
}

void receive(pid_t src, Msg *m) {
    printk("rcv %d:--------receive from %d----------\n", current->pid, src);
    lock();
    printk("list size %d ", list_size(&(current->mes)) );
    NOINTR;
    while (!has_message(current, src)) {// no such message
        // go to sleep
        // if some thread send message to it, it will wake_up this,
        // so return from here and continue
        sleep();
    }
    //P(s);
    NOINTR;
    get_message(current, src, m);
    unlock();
    // following is wrong for system call using send&receive
    //INTR;
    //V(s);
    //printk("%d:-------end receive------\n", current->pid);
}

void init_msg(
    Msg *m,
    pid_t src,
    int tr,
    pid_t r, int d, void *b, off_t o, size_t l) {
    m->src = src;
    m->type = tr;
    m->req_pid = r;
    m->dev_id = d;
    m->buf = b;
    m->offset = o;
    m->len = l;
}