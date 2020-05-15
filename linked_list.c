#include <stdio.h>
#include <stdlib.h>


typedef struct node {
    int val;
    struct node * next;
} node_t;


void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%d\n", current->val);
        current = current->next;
    }
}


node_t * push_back(node_t * head, int val) {
    if (head == NULL) {
        node_t * new_node = (node_t *) malloc(sizeof(node_t));
        new_node->val = val;
        new_node->next = NULL;
        
        return new_node;
    }

    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->val = val;
    current->next->next = NULL;

    return head;
}


node_t * push_front(node_t * head, int val) {
    node_t * new_node = (node_t *) malloc(sizeof(node_t));
    new_node->val = val;
    new_node->next = head;

    return new_node;
}


node_t remove_last(node_t * head) {

    int retval = 0;
    
    /* if there is only one item in the list, remove it */
    if (head->next == NULL) {
        retval = head->val;
        free(head);
        return (node_t) { .val = retval, .next = NULL };
    }

    /* get to the second to last node in the list */
    node_t * current = head;
    while (current->next->next != NULL) {
        current = current->next;
    }

    /* now current points to the second to last item of the list, so let's remove current->next */
    retval = current->next->val;
    current->next = NULL;
    free(current->next);
    return (node_t) { .val = retval, .next = head };
}


node_t remove_first(node_t * head) {

    if (head == NULL)
        return (node_t) { .val = -1, .next = NULL };
    
    node_t * new_head = head->next;
    int retval = head->val;
    free(head);

    return (node_t) { .val = retval, .next = new_head };
}
