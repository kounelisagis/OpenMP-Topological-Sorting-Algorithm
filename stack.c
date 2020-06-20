#include "stack.h"
#include <stdio.h>
#include <stdlib.h>


void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%d\n", current->val);
        current = current->next;
    }
}


node_t * push_front(node_t * head, int val) {
    node_t * new_node = (node_t *) malloc(sizeof(node_t));
    new_node->val = val;
    new_node->next = head;

    return new_node;
}


node_t remove_first(node_t * head) {

    if (head == NULL)
        return (node_t) { .val = -1, .next = NULL };
    
    node_t * new_head = head->next;
    int retval = head->val;
    free(head);

    return (node_t) { .val = retval, .next = new_head };
}
