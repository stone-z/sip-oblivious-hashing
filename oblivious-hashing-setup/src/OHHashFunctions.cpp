#include <cstdio>

using namespace std;

void simpleSum(int* var, int value){
    // var->operator+=value;
    *var = *var + value;
    printf("simpleSumCalled");
}

void simpleSumthingElse(int* var, int value){
    // var->operator+=value;
    *var = *var + value;
    printf("simpleSumthingElseCalled");
}
