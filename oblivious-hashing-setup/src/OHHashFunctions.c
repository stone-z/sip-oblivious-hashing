#include <stdio.h>

void simpleSum(int* var, int value){
    *var = *var + value;
    printf("simpleSumCalled");
}


void simpleSumthingElse(int* var, int value){
    *var = *var + value;
    printf("simpleSumthingElseCalled");
}
