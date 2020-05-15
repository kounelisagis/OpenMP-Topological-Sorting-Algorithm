#include "linked_list.c"
#include <string.h>
#include <omp.h>
#include <sys/time.h>
#include <stdbool.h>


typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;
} graph_node;


node_t * S = NULL;
int L_index = 0;
int * L = NULL;
int num_of_threads = 4;
int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes
graph_node * arr;


void create_task(int node_value) {

    #pragma omp task
    {
    // printf("Thread: %d, node value: %d\n", omp_get_thread_num(), node_value);

    int index;
    #pragma omp critical(L_critical)
    {
    index = L_index++;
    // L = push_front(L, node_value);
    }
    L[index] = node_value;


    node_t * out_node;
    #pragma omp atomic read
    out_node = arr[node_value].out_nodes;


    #pragma omp critical(arr)
    {
    for (; out_node != NULL; out_node = out_node->next) {
        if(--arr[out_node->val].inc_degree == 0) {
            #pragma omp critical(S_critical)
            {
            S = push_front(S, out_node->val);
            }
        }
    }
    }

    }

}


void kahn() {

    #pragma omp parallel
    {

    #pragma omp single
    {

    for (int i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree == 0) {
            S = push_front(S, i);
        }
    }

    bool flag = true;
    if(S == NULL) {
        flag = false;
    }

    while(flag) {

        node_t removed_node;
        #pragma omp critical(S_critical)
        {
        removed_node = remove_first(S);
        S = removed_node.next;  /* .next contains the head - it's used in order to use the same struct */
        }

        int removed_id = removed_node.val;
        create_task(removed_id);

        #pragma omp critical(S_critical)
        {
        if(S == NULL) {
            flag = false;
        }
        }

        if(flag == false) {
            #pragma omp taskwait

            if(S != NULL) {
                flag = true;
            }
        }

    }  // end of while loop

    }  // end of parallel section


    // implicit barrier
    }

    // check for edges
    for(int i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree != 0) {
            printf("Has cycle because of %d which has degree %d!\n", i, arr[i].inc_degree);
            return;
        }
    }


}


void initialize(char * filename) {

    omp_set_num_threads(num_of_threads);

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
        printf("---\nPlease Provide Your Data!\n---\n");
        return -1;
    }


    initialize(argv[1]);



    // run kahn algorithm while measuring the time
    struct timeval start, end;
    gettimeofday(&start, NULL);

    kahn(arr);

    gettimeofday(&end, NULL);
    double delta = (end.tv_sec - start.tv_sec) - (start.tv_usec - end.tv_usec)/1E6;
    printf("%f\n", delta);




    FILE* f = fopen(argv[2], "w");

    for(int i=0;i<L_index;i++) {
        fprintf(f, "%d\n", L[i]);
    }

    free(L);

    fclose(f);

    return 0;
}
