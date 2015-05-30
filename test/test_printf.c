#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

void do_assert(int test, const char* txt) {
  if (!test) {
    write(fileno(stderr), txt, strlen(txt)+1);
    fsync(fileno(stderr));
    exit(1);
  }
}

#define ASSERT(test) do_assert(test, "Assertion failed at " __FILE__ ":" LINE_STRING "; " #test)

void debug(const char *str) {
  write(fileno(stdout), str, strlen(str));
  write(fileno(stdout), "\n", 1);
}

void syscall_print(const char *data, size_t len) {
  (void)(data);
  (void)(len);
  //ERROR!
}

int main() {
  char tmp[512];

  // test empty string
  sprintf(tmp, "");
  ASSERT(strlen(tmp) == 0);

  // test simple string
  sprintf(tmp, "Hello World\n");
  ASSERT(strcmp(tmp, "Hello World\n") == 0);

  // test bare integer
  sprintf(tmp, "%i", 123);
  ASSERT(strcmp(tmp, "123") == 0);

  // test character within format
  sprintf(tmp, "Hello %corld", 'W');
  ASSERT(strcmp(tmp, "Hello World") == 0);

  // We survived simple tests, now test integer sign, padding, and alignment:

  // test signs
  sprintf(tmp, "%i;%i;%+i;%+i;% i;% i", 123, -123, 123, -123, 123, -123);
  ASSERT(strcmp(tmp, "123;-123;+123;-123; 123;-123") == 0);

  // test min.width and sign
  sprintf(tmp, "%5i;%5i;%+5i;%+5i;% 5i;% 5i", 123, -123, 123, -123, 123, -123);
  ASSERT(strcmp(tmp, "  123; -123; +123; -123;  123; -123") == 0);

  // test zero padding
  sprintf(tmp, "%05i;%05i;%0+5i;%0+5i;%0 5i;%0 5i",
          123, -123, 123, -123, 123, -123);
  ASSERT(strcmp(tmp, "00123;-0123;+0123;-0123; 0123;-0123") == 0);

  // test precision
  sprintf(tmp, "%.5i;%.5i;%+.5i;%+.5i;% .5i;% .5i;%.0i",
          123, -123, 123, -123, 123, -123, 0);
  ASSERT(strcmp(tmp, "00123;-00123;+00123;-00123; 00123;-00123;") == 0);

  // test width and precision
  sprintf(tmp, "%7.5i;%7.5i;%+7.5i;%+7.5i;% 7.5i;% 7.5i",
          123, -123, 123, -123, 123, -123);
  ASSERT(strcmp(tmp, "  00123; -00123; +00123; -00123;  00123; -00123") == 0);

  // test left alignment
  sprintf(tmp, "%-7.5i;%-7.5i;%-+7.5i;%-+7.5i;%- 7.5i;%- 7.5i",
          123, -123, 123, -123, 123, -123);
  ASSERT(strcmp(tmp, "00123  ;-00123 ;+00123 ;-00123 ; 00123 ;-00123 ") == 0);

  // test wierd combos
  sprintf(tmp, "% 2.i;%-1.5i;%-+4i;%-+4.2i;%- 1.0i;% .0i",
          123, -123, 123, -123, 123, 0);
  ASSERT(strcmp(tmp, " 123;-00123;+123;-123; 123; ") == 0);

  // integers seem ok, now test unsigned padding and alignment.

  // test min.width, zero padding, width, and precision
  sprintf(tmp, "%5u;%05u;%.5u;%7.5u", 123, 123, 123, 123);
  ASSERT(strcmp(tmp, "  123;00123;00123;  00123") == 0);

  // test wierd combos
  sprintf(tmp, "%2.u;%-1.5u;%-2.0u;%.0u", 123, 123, 0, 0);
  ASSERT(strcmp(tmp, "123;00123;  ;") == 0);

  write(fileno(stdout), "OK\n", 3);
  fsync(fileno(stdout));
  return 0;
}
