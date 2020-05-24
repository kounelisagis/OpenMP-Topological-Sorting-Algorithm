#include "linked_list.c"
#include <string.h>
#include <omp.h>
#include <sys/time.h>
#include <stdbool.h>
#include <time.h>


typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;
} graph_node;


graph_node * arr;


node_t ** S = NULL;
int S_index = -1;

int L_index = 0;
int * L = NULL;

int num_of_threads = 8;
int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes


void create_task(node_t * my_S) {

    #pragma omp task
    {
    // struct timeval start, end, time_diff;
    // gettimeofday(&start, NULL);
    // printf("==%d\n", omp_get_thread_num());

    // printf("Thread: %d, node value: %d\n", omp_get_thread_num(), node_value);
    while(my_S != NULL) {
        node_t removed = remove_first(my_S);
        int last = removed.val;
        my_S = removed.next;

        int index;

        #pragma omp atomic capture
        index = L_index++;

        L[index] = last;


        node_t * out_node = arr[last].out_nodes;

        node_t * to_append = NULL;
        int appended_nodes = 0;


        int s;
        #pragma omp atomic read
        s = S_index;

        bool one_list = s > 8*num_of_threads;


        while(out_node != NULL) {
            int d;

            #pragma omp atomic capture
            d = --arr[out_node->val].inc_degree;

            if(d == 0) {
                if(one_list || appended_nodes%2 == 0)
                    my_S = push_front(my_S, out_node->val);
                else
                    to_append = push_front(to_append, out_node->val);
                appended_nodes++;
            }
            out_node = out_node->next;
        }

        if(to_append != NULL) {

            #pragma omp critical(S_critical)
            S[++S_index] = to_append;
        }

    }
    // gettimeofday(&end, NULL);
    // timersub(&end, &start, &time_diff);
    // printf(" time: %ld.%06ld s\n", time_diff.tv_sec, time_diff.tv_usec);

    }

}


void kahn() {

    #pragma omp parallel 
    {

    #pragma omp single
    {
    printf("-----------%d\n", omp_get_num_threads());
    int count = 0;
    for (int i=1; i<=n_columns; i++) {
        if(arr[i].inc_degree == 0) {
            S[count%(num_of_threads-1)] = push_front(S[count%(num_of_threads-1)], i);
            count++;
        }
    }
    // printf("-=-=-=-=%d\n", count);
    S_index = num_of_threads - 2;

    // for(int i=0;i<S_index+2; i++)
    //     printf("%d: %p\n", i, S[i]);


    bool flag = true;
    if(S_index == -1) {
        flag = false;
    }

    while(flag == true) {
        
        #pragma omp critical(S_critical)
        create_task(S[S_index--]);

        //printf("%d\n", S_index);
        int index;
        #pragma omp atomic read
        index = S_index;

        if(index == -1) {
            flag = false;
        }

        if(flag == false) {
            #pragma omp taskwait

            if(S_index != -1) {
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
    S = (node_t **) malloc(n_columns*sizeof(node_t*));
    
    for(int i=0;i<n_columns;i++)
        S[i] = NULL;
}



int main(int argc, char **argv) {

    if (argc < 3) {
        printf("---\nPlease Provide Your Data!\n---\n");
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


    FILE* f = fopen(argv[2], "w");

    for(int i=0;i<L_index;i++) {
        fprintf(f, "%d\n", L[i]);
    }

    free(L);

    fclose(f);

    return 0;
}

