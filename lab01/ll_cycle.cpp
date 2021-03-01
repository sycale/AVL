#include "ll_cycle.h"

bool ll_has_cycle(struct node *head) {
    struct node *slow_p = head;
    struct node *fast_p = head;
 
    while (slow_p && fast_p && fast_p->next) {
        slow_p = slow_p->next;
        fast_p = fast_p->next->next;
        if (slow_p == fast_p) {
            return 1;
        }
    }
    return 0;
}
