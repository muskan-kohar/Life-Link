#include <stdio.h>
#include <graph.h>

void initialise_Graph(Graph *g){
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

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            g->mat[i][j] = matrix[i][j];
        }
    }
}

void get_Compatible_Groups(Graph* g, BloodGroup patient, BloodGroup compatible[], int   *count){
    *count = 0;
    for(int i = 0; i <8; i++){
        if(g->mat[patient][i] == 1){
            compatible[*count] = i;
            (*count)++;
        }
    }
}
BloodGroup convert_To_enum(char input[]){
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
char* bloodGroupName(BloodGroup group){
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
int main(){
    Graph g;

    initialise_Graph(&g);

    printf("Enter the blood groups you want to find the compatible groups for: \n");
    printf("0 -> O-\n1 -> O+\n2 -> A-\n3 -> A+\n4 -> B-\n5 -> B+\n6 -> AB-\n7 -> AB+\n");
    BloodGroup patient;
    scanf("%d", patient);
    BloodGroup compatible[8];
    int count;
    get_Compatible_Groups(&g, patient, compatible, &count);

}