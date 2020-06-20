#include "stack.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;
} graph_node;


// an array containing the graph info needed
graph_node * nodes;


int L_index = 0;
int * L = NULL;

int num_of_threads;
int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes


void create_task(int node) {

    #pragma omp task
    {
    node_t * my_S = NULL;
    my_S = push_front(my_S, node);


    while(my_S != NULL) {
        node_t removed = remove_first(my_S);
        int last = removed.val;
        my_S = removed.next;

        int index;

        #pragma omp atomic capture
        index = L_index++;

        L[index] = last;


        node_t * out_node = nodes[last].out_nodes;

        int counter = 0;

        while(out_node != NULL) {
            int d;

            #pragma omp atomic capture
            d = --nodes[out_node->val].inc_degree;

            if(d == 0) {
                if(counter%2==0)
                    create_task(out_node->val);
                else
                    my_S = push_front(my_S, out_node->val);
                
                counter++;
            }

            out_node = out_node->next;
        }

    }

    }

}


void kahn() {

    #pragma omp parallel 
    {
    #pragma omp single
    printf("Number of threads: %d\n", omp_get_num_threads());


    #pragma omp for
    for(int i=1;i<=n_columns;i++) {
        if(nodes[i].inc_degree == 0) {
            create_task(i);
        }
    }

    // implicit barrier
    }

    // check for edges
    for(int i=1;i<=n_columns;i++) {
        if(nodes[i].inc_degree != 0) {
            printf("Has cycle because of %d, which has degree %d!\n", i, nodes[i].inc_degree);
            return;
        }
    }


}


void initialize(char * filename, char * threads_selected) {
    num_of_threads = atoi(threads_selected);

    omp_set_num_threads(num_of_threads);

    FILE* f = fopen(filename, "r");
    char *line_buf = NULL;
    size_t line_buf_size = 0;

    // ignore comment lines
    while ((getline(&line_buf, &line_buf_size, f)) != -1 && line_buf[0] == '%');

    // read the the first meaningful line - rows, columns, edges
    sscanf(line_buf, "%d %d %d", &n_rows, &n_columns, &n_edges);

    // memory allocation - position 0 is NOT used
    nodes = (graph_node *) malloc((n_columns+1)*sizeof(graph_node));

    // nodesay initialization
    for(int i=1;i<=n_rows;i++) {
        nodes[i] = (graph_node) { .inc_degree = 0, .out_nodes = NULL };
    }

    // graph representation construction
    for(int i=1; i<=n_edges; i++) {
        int node_out, node_in;
        if(fscanf(f, "%d %d\n", &node_out, &node_in) == 2)

        nodes[node_out].out_nodes = push_front(nodes[node_out].out_nodes, node_in);
        nodes[node_in].inc_degree++;
    }

    fclose(f);

    L = (int *) malloc(n_columns*sizeof(int));
}



int main(int argc, char **argv) {

    if (argc < 4) {
        printf("---\nPlease provide your data in the corrent form!\n---\n");
        return -1;
    }

    initialize(argv[1], argv[3]);


    // run kahn algorithm while measuring the time
    struct timeval start, end, time_diff;
    gettimeofday(&start, NULL);

    kahn();

    gettimeofday(&end, NULL);


    timersub(&end, &start, &time_diff);
    printf("Elapsed time: %ld.%06ld s\n", time_diff.tv_sec, time_diff.tv_usec);


    FILE* f = fopen(argv[2], "w");

    for(int i=0;i<L_index;i++) {
        fprintf(f, "%d\n", L[i]);
    }

    fclose(f);
    free(L);
    free(nodes);


    return 0;
}
