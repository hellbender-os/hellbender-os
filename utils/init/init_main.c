#include <stdio.h>

extern char** environ;

int main(int argc, char** argv) {

  printf("Init argc = %i\n", argc);
  for (int i = 0; i < argc; ++i) {
    printf("Init argv = '%s'\n", argv[i]);
  }
  for (char** ptr = environ; *ptr; ptr++) {
    printf("Env: '%s'\n", *ptr);
  }
  return 2;
}
