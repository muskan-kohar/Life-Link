#include <stdio.h>
#include <string.h>
#include "graph.h"

void initialise_Graph(Graph *g){
    if (g == NULL)
        return;

    /* Rows are patient groups; columns mark compatible donor groups. */
    int matrix[8][8] = {
       //O- O+ A- A+ B- B+ AB-AB+
        {1, 0, 0, 0, 0, 0, 0, 0}, //O-
        {1, 1, 0, 0, 0, 0, 0, 0}, //O+
        {1, 0, 1, 0, 0, 0, 0, 0}, //A-
        {1, 1, 1, 1, 0, 0, 0, 0}, //A+
        {1, 0, 0, 0, 1, 0, 0, 0}, //B-
        {1, 1, 0, 0, 1, 1, 0, 0}, //B+
        {1, 0, 1, 0, 0, 0, 1, 0}, //AB-
        {1, 1, 1, 1, 1, 1, 1, 1}  //AB+
    };

    /* Copy the compatibility table into the graph. */
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            g->mat[i][j] = matrix[i][j];
        }
    }
}

/* Fill compatible[] with donor groups accepted by the patient's group. */
void get_Compatible_Groups(const Graph *g, BloodGroup patient, BloodGroup compatible[], int *count){
    if (count == NULL)
        return;

    *count = 0;
    /* Reject invalid inputs before using the blood group as a matrix index. */
    if (g == NULL || compatible == NULL || patient < O_NEG || patient > AB_POS)
        return;

    for (int i = 0; i < 8; i++){
        if (g->mat[patient][i] == 1) {
            compatible[*count] = i;
            (*count)++;
        }
    }
}

/* Convert a blood-group label to its enum value, or -1 if unrecognized. */
BloodGroup convert_To_enum(const char *input){
    if (input == NULL)
        return (BloodGroup)-1;
    if(strcmp(input, "O-") == 0){
        return O_NEG;
    }
    if(strcmp(input, "O+") == 0){
        return O_POS;
    }
    if(strcmp(input, "A-") == 0){
        return A_NEG;
    }
    if(strcmp(input, "A+") == 0){
        return A_POS;
    }
    if(strcmp(input, "B-") == 0){
        return B_NEG;
    }
    if(strcmp(input, "B+") == 0){
        return B_POS;
    }
    if(strcmp(input, "AB-") == 0){
        return AB_NEG;
    }
    if(strcmp(input, "AB+") == 0){
        return AB_POS;
    }

    return -1;
}

/* Return the printable label for a blood-group enum value. */
const char *bloodGroupName(BloodGroup group){
    switch(group){
        case O_NEG:
            return "O-";

        case O_POS:
            return "O+";

        case A_NEG:
            return "A-";

        case A_POS:
            return "A+";

        case B_NEG:
            return "B-";

        case B_POS:
            return "B+";

        case AB_NEG:
            return "AB-";

        case AB_POS:
            return "AB+";     
    }

    return "Invalid";
}
