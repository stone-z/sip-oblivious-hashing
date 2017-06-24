#include <stdio.h>

void simpleSum(int* var, int value){
    *var = *var + value;
    printf("simpleSumCalled");
}


void simpleSumthingElse(int* var, int value){
    *var = *var + 2 * value;
    printf("simpleSumthingElseCalled");
}

int assertEqual(int* var, int value){
    printf("Assert: Address: <%p> | Expect: <%d> | Current: <%d> \n", 
    var, value, *var);
    return 1;
}
