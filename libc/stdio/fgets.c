#include <stdio.h>

char *fgets(char *str, int size, FILE *file) {
  int i = 0;
  while (i < size - 1) {
    int ch = fgetc(file);
    if (ch == EOF) {
      file->eof = 1;
      break;
    }
    str[i++] = (char)ch;
    if (ch == '\n') break;
  }
  if (!i) return NULL;
  str[i] = 0;
  return str;
}

