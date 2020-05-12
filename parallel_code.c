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


void kahn(graph_node *arr) {

    int i;
    for (i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree == 0) {
            S = push(S, i);
        }
    }

    #pragma omp parallel shared(arr)
    {

    #pragma omp single
    {
    while (S != NULL)
    {
        while(S != NULL) {

            // print_list(S);
            // printf("---\n");

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
                #pragma omp task
                {
                // printf("Thread: %d, in_value: %d, out_value: %d, degree: %d\n", omp_get_thread_num(), removed_id, out_nodes->val, arr[out_nodes->val].inc_degree);

                int d;

                #pragma omp critical(inc_degree_critical)
                {
                d = --arr[out_nodes->val].inc_degree;
                }

                if(d == 0) {
                    #pragma omp critical(S_critical)
                    {
                    S = push(S, out_nodes->val);
                    }
                }
                
                }

                out_nodes = out_nodes->next;
            }
        }

        if(S==NULL)
        {
            // printf("Hello1\n");
            #pragma omp taskwait
        }
        // printf("Hello2\n");

        // proccessNode(arr);
    }
    // implicit barrier
    }
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




int main() {
    /* setting the number of threads */
    omp_set_num_threads(num_of_threads);

    /* start of data read area */

    FILE* f = fopen("dag.txt", "r");

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
    graph_node arr[n_columns+1];

    int i;
    for(i=1;i<=n_rows;i++) {
        arr[i] = (graph_node) { .inc_degree = 0, .out_nodes = NULL };
    }

    for(i=1; i<=n_edges; i++) {
        int node_out, node_in;
        fscanf(f, "%d %d\n", &node_out, &node_in);

        arr[node_out].out_nodes = push(arr[node_out].out_nodes, node_in);
        arr[node_in].inc_degree++;
    }

    fclose(f);


    /* end of data read area */


    /* call of the main function */
    gettimeofday(&start, NULL);

    kahn(arr);

    gettimeofday(&end, NULL);


    double delta = (end.tv_sec - start.tv_sec) - (start.tv_usec- end.tv_usec)/1E6;

    printf("%f\n", delta);

    // strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&delta));
    // printf("%s%ld\n",buffer,tv.tv_usec);

    // printf("Final List:\n");
    // print_list(L);


    // printf("------------------------------");

    // for(i=1;i<=n_rows;i++) {
    //     printf("i: %d | inc: %d | out:\n", i, arr[i].inc_degree);
    //     print_list(arr[i].out_nodes);
    // }


    // /* output print */
    // f = fopen("output1.txt", "w");

    // while(L != NULL) {
    //     fprintf(f, "%d\n", L->val);
    //     L = L->next;
    // }

    // fclose(f);


    return 0;
}