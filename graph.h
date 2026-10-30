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
/* Fill compatible[] with donor groups accepted by patient and set *count. */
void get_Compatible_Groups(const Graph *g, BloodGroup patient,
                           BloodGroup compatible[], int *count);
/* Convert a blood-group label to an enum, or return (BloodGroup)-1. */
BloodGroup convert_To_enum(const char *input);
/* Get the printable label for a group, or "Invalid" for an unknown value. */
const char *bloodGroupName(BloodGroup group);

#endif
