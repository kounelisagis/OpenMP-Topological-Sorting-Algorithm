#include "linked_list.c"
#include <string.h>
#include <omp.h>
#include <sys/time.h>
#include <stdbool.h>


typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;
} graph_node;


node_t ** S = NULL;
int num_of_threads = 4;
int * L = NULL;
int L_index = 0;
int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes
graph_node * arr;



bool are_all_null(){
    for
}


void create_task(int node_value) {

    #pragma omp task
    {
    // printf("Thread: %d, node value: %d\n", omp_get_thread_num(), node_value);

    int index;
    #pragma omp critical(L_critical)
    {
    index = L_index++;
    }
    L[index] = node_value;


    node_t * out_node;
    #pragma omp atomic read
    out_node = arr[node_value].out_nodes;

    int tid = omp_get_thread_num();

    #pragma omp critical(arr)
    {
    for (; out_node != NULL; out_node = out_node->next) {
        if(--arr[out_node->val].inc_degree == 0) {
            S[tid] = push_front(S[tid], out_node->val);          // 1---->2   4------>2     5------>2
        }
    }
    }

    }

}


void kahn() {

    #pragma omp parallel
    {

    int tid = omp_get_thread_num();

    #pragma omp for
    for (int i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree == 0) {
            S[tid] = push_front(S[tid], i);
        }
    }
    // barrier


    while(S[tid] != NULL)
    {
        node_t removed_node;
        removed_node = remove_first(S[tid]);
        S[tid] = removed_node.next;  /* .next contains the head - it's used in order to use the same struct */

        int removed_id = removed_node.val;
        create_task(removed_id);


        // 
        // if(S[tid] == NULL) {
        //     int result;
        //     #pragma omp critical(flag_area)
        //     {
        //         #pragma omp taskwait
        //         flags[tid] = false;
        //         result = are_all_null();
        //     }
            
        //     while(!result) {
        //         #pragma omp critical(flag_area)
        //         {   
        //             result = are_all_null();
        //         }
        //     }
        // }
        
        }
        
    }

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

    // ignore comment lines
    while ((getline(&line_buf, &line_buf_size, f)) != -1 && line_buf[0] == '%');

    // read the the first meaningful line
    sscanf(line_buf, "%d %d %d", &n_rows, &n_columns, &n_edges);

    /*
        arr contains the nodes of the graph
        arr[0] will not be used
    */

    S = (node_t **) malloc(num_of_threads*sizeof(node_t *));
    L = (int *) malloc((n_columns)*sizeof(int));
    arr = (graph_node *) malloc((n_columns+1)*sizeof(graph_node));

    for(int i=1; i<=n_rows; i++) {
        arr[i] = (graph_node) { .inc_degree = 0, .out_nodes = NULL };
    }

    for(int i=1; i<=n_edges; i++) {
        int node_out, node_in;
        if(fscanf(f, "%d %d\n", &node_out, &node_in) == 2)

        arr[node_out].out_nodes = push_front(arr[node_out].out_nodes, node_in);
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

    // print_list(L);

    // for(int i=0;i<n_columns;i++) {
    //     printf("%d\n", L[i]);
    // }

    printf("%f\n", delta);

    return 0;
}