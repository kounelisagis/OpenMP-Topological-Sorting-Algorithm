#include <stdio.h>
#include "linked_list.c"
#include <string.h>
#include <omp.h>

#include <time.h>

typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;

} graph_node;


int not = 2;
node_t * L = NULL;
node_t * S[] = {NULL, NULL};
int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes


void proccessNode(graph_node * arr) {
    int tid = omp_get_thread_num();

    if (S[tid] == NULL)
        return;

    node_t removed = remove_last(S[tid]); // 5 7
    int last = removed.val;
    S[tid] = removed.next;

    node_t * out_node;
    #pragma omp critical
    {
    L = push(L, last); // 3
    out_node = arr[last].out_nodes;
    printf("last: %d\n", last);
    print_list(arr[last].out_nodes);
    printf("S[tid]: %d\n", S[tid]);
    printf("S: %d\n", S);
    }

    while(out_node != NULL)
    {
        #pragma omp task
        {
        // printf("Thread: %d, out_value: %d, degree: %d\n", tid, out_node->val, arr[out_node->val].inc_degree);

        int d;

        #pragma omp critical
        {
        arr[out_node->val].inc_degree--;
        d = arr[out_node->val].inc_degree;
        }

        if(d == 0) {
            S[tid] = push(S[tid], out_node->val);
        }

        // #pragma omp taskwait
        proccessNode(arr);
        }

        out_node = out_node->next;
    }

}



void kahn(graph_node *arr) {

    #pragma omp parallel shared(L, arr)
    {
    // node_t * S = NULL;

    int i;

    #pragma omp for
    for (i=1;i<=n_columns;i++) {
        int tid = omp_get_thread_num();
        // int nthreads = omp_get_num_threads();
        // printf("Thread: %d / %d | i: %d\n", tid, nthreads, i);

        if(arr[i].inc_degree == 0) {
            S[tid] = push(S[tid], i);
        }
    }

    // printf("List of thread #%d:\n", omp_get_thread_num());
    // print_list(S[omp_get_thread_num()]);

    proccessNode(arr);

    #pragma omp barrier

    }
    
    // printf("%d\n", L);
    // print_list(L);

    int i;
    // for(i=1; i<=n_columns; i++) {
    //     printf("node: %d, degree: %d\n", i , arr[i].inc_degree);
    // }


    // check for edges
    for(i=1;i<=n_columns;i++) {
        if(arr[i].inc_degree != 0) {
            printf("Has cycle because of %d which has degree %d!\n", i, arr[i].inc_degree);
            return;
        }
    }

}



int main() {
    omp_set_num_threads(not);

    double cpu_time_used;
    clock_t start, end;

    FILE* f = fopen("datasets/input.mtx", "r");

    char *line_buf = NULL;
    size_t line_buf_size = 0;

    getline(&line_buf, &line_buf_size, f);

    while(line_buf[0] == '%') {
        getline(&line_buf, &line_buf_size, f);
    }

    sscanf(line_buf, "%d %d %d", &n_rows, &n_columns, &n_edges);


    start = clock();

    graph_node arr[n_columns+1];

    int i;


	#pragma omp parallel for
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

    kahn(arr);

    end = clock();



    f = fopen("output1.txt", "w");

    while(L != NULL) {
        fprintf(f, "%d\n", L->val);
        L = L->next;
    }

    fclose(f);


    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%lf\n",cpu_time_used);

    return 0;
}