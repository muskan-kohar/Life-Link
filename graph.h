#ifndef GRAPH_H
#define GRAPH_H

/* Compatibility matrix: rows are patient groups and columns are donor groups. */
typedef struct {
    int mat[8][8];
} Graph;

/* Enum order matches the compatibility-matrix row and column indexes. */
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

/* Initialize the red-cell compatibility matrix. */
void initialise_Graph(Graph* g);
void get_Compatible_Groups(Graph* g, BloodGroup patient, BloodGroup compatible[], int *count);
BloodGroup convert_To_enum(char input[]);
char* bloodGroupName(BloodGroup group);


#endif
