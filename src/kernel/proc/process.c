#include "process.h"
#include "../mem_manager/alloc/alloc.h"

#define STACK_SIZE 8192

static proc_t *head;
static proc_t *current;
static u64 next_pid;

void process_init(void) {
    head = NULL;
    current = NULL;
    next_pid = 1;
}

proc_t *process_create(const char *name, u64 entry) {
    proc_t *p = (proc_t *)kalloc(sizeof(proc_t));
    if (!p) return NULL;

    u64 stk = (u64)kalloc(STACK_SIZE);
    if (!stk) {
        kfree(p);
        return NULL;
    }

    p->pid = next_pid++;
    p->state = PROC_READY;
    p->stack_base = stk;
    p->stack_ptr = stk + STACK_SIZE;
    p->entry = entry;
    p->next = head;

    int i = 0;
    while (name[i] && i < 63) {
        p->name[i] = name[i];
        i++;
    }
    p->name[i] = '\0';

    head = p;
    return p;
}

void process_destroy(proc_t *p) {
    if (!p) return;

    proc_t *cur = head, *prev = NULL;

    while (cur) {
        if (cur == p) {
            if (prev) prev->next = cur->next;
            else head = cur->next;

            if (current == p) current = NULL;

            kfree((void *)p->stack_base);
            kfree(p);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

proc_t *process_get_current(void) {
    return current;
}

void process_set_state(proc_t *p, proc_state_t state) {
    if (p) p->state = state;
}
