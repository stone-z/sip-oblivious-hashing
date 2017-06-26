#include <stdio.h>
#include <stdlib.h>

void simpleSum(int* var, int value){
    *var = *var + value;
}


void simpleSumthingElse(int* var, int value){
    *var = *var + 2 * value;
}

int assertEqual(int* var, int value){
    printf("Assert: Address: <%p> | Expect: <%x> | Current: <%x> \n", 
    var, value, *var);
    if(value != *var) {
       printf("Program corrupted! Exiting!\n");
       exit(1);
    }
    return 1;
}
