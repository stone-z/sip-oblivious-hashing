#include <stdio.h>

int encrypt(int input) {
   int multiply = 3;
   int sum = 2;
   int output = input * multiply + sum;
return output;
}

void notify(int output) {
   if(output < 4) {
      printf("The small output of calc is: %d.\n", output);
   } else {
      printf("The big output of calc is: %d.\n", output);
   }
}

int calc(int input) {
   int sum = 15;
   int output = input + sum;
   output -= 1;
   return output;
}

int operation2(int input) {
   int output = encrypt(input);
   return output;
}

int operation1(int input) {
   int output = calc(input);
   notify(output);

return output;
}

void loadUI() {
   int a = 1;
   int b = 2;
   if(a < b) {
      printf("Doing some magic here.\n");
   } else {
      printf("Doing magic anyway.\n");
   }
}

int main() {
  loadUI();
  int res1 = operation1(2);
  int res2 = operation2(2);
}
