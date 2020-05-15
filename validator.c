#include "linked_list.c"
#include <string.h>
#include <sys/time.h>


typedef struct graph_node {
    int inc_degree;
    node_t * out_nodes;

} graph_node;


int n_rows, n_columns, n_edges; //number of rows and cols of the matrix and the nodes


int main(int argc, char **argv) {

    FILE* f;

    if (argc > 2) {
        f = fopen(argv[1], "r");
    }
    else {
        printf("---\nPlease Provide Your Data!\n---\n");
        return -1;
    }
    

    char *line_buf = NULL;
    size_t line_buf_size = 0;

    // ignore comment lines
    while ((getline(&line_buf, &line_buf_size, f)) != -1 && line_buf[0] == '%');

    // read the the first meaningful line
    sscanf(line_buf, "%d %d %d", &n_rows, &n_columns, &n_edges);


    graph_node *arr = (graph_node *) malloc((n_columns+1)*sizeof(graph_node));


    for(int i=1;i<=n_rows;i++) {
        arr[i] = (graph_node) { .inc_degree = 0, .out_nodes = NULL };
    }

    for(int i=1; i<=n_edges; i++) {
        int node_out, node_in;
        if(fscanf(f, "%d %d\n", &node_out, &node_in) == 2)

        arr[node_out].out_nodes = push_front(arr[node_out].out_nodes, node_in);
        arr[node_in].inc_degree++;
    }

    fclose(f);


    f = fopen(argv[2], "r");

    int node;
    while(fscanf(f, "%d\n", &node) == 1) {
        if(arr[node].inc_degree == 0) {
            for (node_t * out_node = arr[node].out_nodes; out_node != NULL; out_node = out_node->next) {
                --arr[out_node->val].inc_degree;
            }
        } else {
            printf("WRONG topological sort\n");
            return -1;
        }
    }
    
    printf("CORRECT topological sort\n");
    return 0;
}
