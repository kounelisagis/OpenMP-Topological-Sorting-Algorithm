#include "linked_list.c"
#include <string.h>
#include <omp.h>
#include <sys/time.h>


typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;
} graph_node;


node_t * S = NULL;
int num_of_threads = 8;
node_t * L = NULL;
int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes
int num_of_tasks = 0;
graph_node * arr;


void create_task(int node_value) {

    #pragma omp task
    {
    // printf("Thread: %d, in_value: %d, out_value: %d, degree: %d\n", omp_get_thread_num(), removed_id, out_nodes->val, arr[out_nodes->val].inc_degree);

    int d;

    #pragma omp critical(inc_degree_critical)
    {
    d = --arr[node_value].inc_degree;
    }

    if(d == 0) {
        #pragma omp critical(S_critical)
        {
        S = push(S, node_value);
        }
    }
    
    }

}


void kahn() {

    int i;
    for (i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree == 0) {
            S = push(S, i);
        }
    }

    #pragma omp parallel
    {

    #pragma omp single
    {
    while (S != NULL)
    {
        node_t removed_node;
        #pragma omp critical(S_critical)
        {
        removed_node = remove_last(S);
        S = removed_node.next; /* .next contains the head - in order to use the same struct */
        }
        int removed_id = removed_node.val;

        L = push(L, removed_id);

        node_t * out_nodes = arr[removed_id].out_nodes;

        while(out_nodes != NULL)
        {
            create_task(out_nodes->val);
            out_nodes = out_nodes->next;
        }

        if(S == NULL) {
            #pragma omp taskwait
        }
        // #pragma omp taskwait

    }

    // proccessNode(arr);
    }
    // implicit barrier
    }


    // check for edges
    for(i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree != 0) {
            printf("Has cycle because of %d which has degree %d!\n", i, arr[i].inc_degree);
            return;
        }
    }

    // print_list(L);
}


int main(int argc, char **argv) {

    FILE* f;

    if (argc > 1) {
        f = fopen(argv[1], "r");
    }
    else {
        printf("---\nPlease Provide Your Data!\n---\n");
        return -1;
    }

    /* setting the number of threads */
    omp_set_num_threads(num_of_threads);

    /* start of data read area */

    char *line_buf = NULL;
    size_t line_buf_size = 0;

    getline(&line_buf, &line_buf_size, f);

    while(line_buf[0] == '%') {
        getline(&line_buf, &line_buf_size, f);
    }

    sscanf(line_buf, "%d %d %d", &n_rows, &n_columns, &n_edges);

    /*
        arr contains the nodes of the graph
        arr[0] will not be used
    */

    arr = (graph_node *) malloc((n_columns+1)*sizeof(graph_node));

    for(int i=1; i<=n_rows; i++) {
        arr[i] = (graph_node) { .inc_degree = 0, .out_nodes = NULL };
    }

    for(int i=1; i<=n_edges; i++) {
        int node_out, node_in;
        fscanf(f, "%d %d\n", &node_out, &node_in);

        arr[node_out].out_nodes = push(arr[node_out].out_nodes, node_in);
        arr[node_in].inc_degree++;
    }

    fclose(f);


    /* end of data read area */


    /* call of the main function */
    struct timeval start, end;
    gettimeofday(&start, NULL);

    kahn();

    gettimeofday(&end, NULL);


    double delta = (end.tv_sec - start.tv_sec) - (start.tv_usec- end.tv_usec)/1E6;

    printf("%f\n", delta);

    return 0;
}