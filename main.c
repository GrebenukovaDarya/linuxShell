#include <string.h>
#include <stdio.h>
int main() {

  printf("$ ");

  fflush(stdout);

  char input[100];

  fgets(input, 100, stdin);
 
  input[strlen(input) - 1] = '\0';

  printf("%s: command not found\n", input);

  return 0;

}
