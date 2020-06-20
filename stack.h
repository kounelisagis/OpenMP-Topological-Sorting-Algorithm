#ifndef HEADER_FILE
#define HEADER_FILE


typedef struct node {
    int val;
    struct node * next;
} node_t;


node_t * push_front(node_t *, int);
node_t remove_first(node_t *);


#endif
