#ifndef GRAPH_H
#define GRAPH_H

typedef struct {
    int mat[8][8];
}Graph;
typedef enum{
    O_NEG,
    O_POS,
    A_NEG,
    A_POS,
    B_NEG,
    B_POS,
    AB_NEG,
    AB_POS
}BloodGroup;
void initialise_Graph(Graph* g);
void get_Compatible_Groups(Graph* g, int patient, int compatible, int *count);




#endif
