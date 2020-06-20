#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;

} graph_node;

int L_index = 0;
int * L = NULL;
graph_node *arr;

int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes


void kahn(graph_node *arr) {
    node_t * S = NULL;

    int i;
    for (i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree == 0) {
          S = push_front(S, i);
        }
    }


    while(S != NULL) {
        node_t removed = remove_first(S);
        int last = removed.val;
        S = removed.next;

        L[L_index++] = last;

        node_t * out_node = arr[last].out_nodes;
        while(out_node != NULL)
        {
            arr[out_node->val].inc_degree--;
            if(arr[out_node->val].inc_degree == 0) {
                S = push_front(S, out_node->val);
            }
            out_node = out_node->next;
        }
    }

    // check for edges
    for(i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree != 0) {
            printf("Has cycle!\n");
        }
    }

}


void initialize(char * filename) {

    FILE* f = fopen(filename, "r");
    char *line_buf = NULL;
    size_t line_buf_size = 0;

    // ignore comment lines
    while ((getline(&line_buf, &line_buf_size, f)) != -1 && line_buf[0] == '%');

    // read the the first meaningful line - rows, columns, edges
    sscanf(line_buf, "%d %d %d", &n_rows, &n_columns, &n_edges);

    // memory allocation - position 0 is NOT used
    arr = (graph_node *) malloc((n_columns+1)*sizeof(graph_node));

    // array initialization
    for(int i=1;i<=n_rows;i++) {
        arr[i] = (graph_node) { .inc_degree = 0, .out_nodes = NULL };
    }

    // graph representation construction
    for(int i=1; i<=n_edges; i++) {
        int node_out, node_in;
        if(fscanf(f, "%d %d\n", &node_out, &node_in) == 2)

        arr[node_out].out_nodes = push_front(arr[node_out].out_nodes, node_in);
        arr[node_in].inc_degree++;
    }

    fclose(f);

    L = (int *) malloc(n_columns*sizeof(int));
}



int main(int argc, char **argv) {

    if (argc < 3) {
        printf("---\nPlease provide your data in the corrent form!\n---\n");
        return -1;
    }


    initialize(argv[1]);


    // run kahn algorithm while measuring the time
    struct timeval start, end, time_diff;
    gettimeofday(&start, NULL);

    kahn(arr);

    gettimeofday(&end, NULL);
    timersub(&end, &start, &time_diff);
    printf("Elapsed time: %ld.%06ld s\n", time_diff.tv_sec, time_diff.tv_usec);


    // write results and free L

    FILE* f = fopen(argv[2], "w");


    for(int i=0; i<n_columns; i++) {
        fprintf(f, "%d\n", L[i]);
    }
    
    fclose(f);


    // free array memory
    free(arr);
    free(L);


    return 0;
}
