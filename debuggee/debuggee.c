#include <stdio.h>

int g_n_iter = 6;

int NumOfIter() {
  return g_n_iter - 1;
}

int main() {
  int i = 0;
  for (; i < NumOfIter(); i++) {
    printf("#%d : Hello World!\n", i);
  }
  return 0;
}
