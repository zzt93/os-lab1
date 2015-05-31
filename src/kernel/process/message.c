#include "kernel/message.h"
#include "kernel/process.h"
#include "kernel/semaphore.h"
#include "lib/string.h"
#include "lib/malloc.h"



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
    printk("#%d: src %d, dest %d\n", current->pid, tmp->src, tmp->dest);
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
    printk("-------#%d to send src: %d to dest: %d------", current->pid, m->src, dest);
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
    printk("%d:--------receive from %d----------\n", current->pid, src);
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
    INTR;
    //V(s);
    //printk("%d:-------end receive------\n", current->pid);
}