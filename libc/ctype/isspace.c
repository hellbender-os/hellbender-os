#include <ctype.h>

int isspace(int c) {
  switch (c) { // For POSIC locale, include:
  case ' ':    //  <space>
  case '\f':   //  <form-feed>
  case '\n':   //  <newline>
  case '\r':   //  <carriage-return>
  case '\t':   //  <tab>
  case '\v':   //  <vertical-tab>
    return 1;
  default:
    return 0;
  }
}
