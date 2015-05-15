#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <hellbender.h>
#include <coresrv/rtc.h>
#include <coresrv/kbd.h>
#include <coresrv/vga.h>
#include <sys/keymap.h>

keymap_t* keymap;

const char builtin_help[] = "help";
const char builtin_pwd[]  = "pwd";
const char builtin_cd[]   = "cd";
const char builtin_ls[]   = "ls";
const char builtin_cat[]  = "cat";

#define PATH_SEPARATOR '/'
char cwd[PATH_MAX] = "";

int is_whitespace(char c) {
  switch (c) {
  case 0x09:
  case 0x0a:
  case 0x0b:
  case 0x0c:
  case 0x0d:
  case 0x20:
    return 1;
  default:
    return 0;
  }
}

char* trim(char* str) {
  // trim leading:
  while (*str && is_whitespace(*str)) ++str;
  // find end:
  char *end = str;
  while (*end) ++end;
  // trim tailing:
  while (end > str && (!*end || is_whitespace(*end))) --end;
  end[1] = 0;
  return str;
}

void normalize_cwd() {
  // remove '//', '/./', '/../'
  char *src = cwd;
  char *dst = cwd;
  while (*src) {
    if (strncmp(src, "//", 2) == 0) ++src;
    else if (strncmp(src, "/./", 3) == 0) src += 2;
    else if (strncmp(src, "/../", 4) == 0) {
      while (dst > cwd && *(--dst) != PATH_SEPARATOR);
      src += 3;
    } else {
      *dst++ = *src++;
    }
  }
  *dst = 0;
}

void test_timer() {
  unsigned counter = 0;
  unsigned last = 0;
  while (counter < 200) {
    counter = CORE_IDC(coresrv_rtc_ticks);
    if (counter == last) {
      sched_yield();
      continue;
    }
    last = counter;
    if (counter % 20 == 0) {
      printf("tick\n");
    }
  }
}

void read_command(char *buf, size_t size) {
  uint8_t color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
  unsigned x = strlen(cwd) + 2;
  unsigned y = 24;
  unsigned n = 0;
  unsigned max_n = 79 - x;
  if (size < max_n + 1) abort();

  // prompt, and clear the line.
  printf(cwd);
  CORE_IDC(coresrv_vga_putc, x-2, y, x, '>', color);
  for (unsigned i = x; i < 80; ++i)
    CORE_IDC(coresrv_vga_putc, i, y, x, ' ', color);

  // collect key presses until new-line.
  while (1) {
    int kc = CORE_IDC(coresrv_kbd_getc);
    if (kc < 0) {
      sched_yield();
      continue;
    }
    int c = keymap_code2char(keymap, KBD_GETC_KEYCODE(kc), KBD_GETC_FLAGS(kc));
    if (c <= 0) continue;
    //if (c <= 0) {
    //  printf("code %x = %c\n", (unsigned)KBD_GETC_KEYCODE(kc), (unsigned)c);
    //  continue;
    //}
    if (c == '\b') {
      if (n) {
        --n;
        CORE_IDC(coresrv_vga_putc, x + n, y, x + n, ' ', color);
      }
      continue;
    }
    if (c == '\n') {
      buf[n] = '\0';
      return;
    }
    if (n < max_n) {
      CORE_IDC(coresrv_vga_putc, x + n, y, x + n + 1, c, color);
      buf[n] = c;
      ++n;
      continue;
    }
  }
}

void do_help() {
  printf("Available commands:\n");
  printf("  help        gives this list.\n");
  printf("  pwd         prints current working directory.\n");
  printf("  cd <dir>    changes working direcory (\"cd\" goes root; \"cd ..\" goes up).\n");
  printf("  ls          lists the contents of current working directory.\n");
  printf("  cat <file>  outputs file to the terminal.\n");
}

void do_pwd() {
  printf("%s\n", cwd);
}

void do_cd(char *dir) {
  dir = trim(dir);
  unsigned cwd_len = strlen(cwd);
  unsigned dir_len = strlen(dir);
  if (dir_len == 0) { // empty cd resets to root.
    cwd[0] = 0;
    
  } else if (dir_len > 0) {
    strcpy(dir + dir_len, "/"); // make sure paths end with a separator.
    ++dir_len;
    if (dir[0] == PATH_SEPARATOR) { // leading separator is absolute path.
      strcpy(cwd, dir);
      
    } else { // all else are relative paths.
      if (cwd_len + dir_len + 2 < PATH_MAX) {
        cwd[cwd_len] = PATH_SEPARATOR;
        strcpy(cwd + cwd_len + 1, dir);
      } else {
        printf("Error: max path length exceeded.\n");
      }
    }
  }
  normalize_cwd();
}

void do_ls() {
  int dir = open(cwd, O_RDONLY);
  if (dir >= 0) {
    struct dirent dirent;
    while (read(dir, &dirent, sizeof(dirent)) == sizeof(dirent)) {
      printf("%s\n", dirent.d_name);
    }
    close(dir);
  } else {
    printf("Error: failed to open directory '%s'\n", cwd);
  }
}

void do_cat(char* file) {
  size_t cwd_len = strlen(cwd);
  size_t file_len = strlen(file);
  if (cwd_len + file_len < PATH_MAX) {
    char file_path[PATH_MAX];
    strcpy(file_path, cwd);
    strcpy(file_path + cwd_len, file);

    int file = open(file_path, O_RDONLY);
    if (file >= 0) {
      char c;
      while (read(file, &c, sizeof(c)) == sizeof(c)) {
        printf("%c", c);
      }
    } else {
      printf("Error: failed to open file '%s'\n", file_path);
    }
    
  } else {
    printf("Error: max path length exceeded.\n");
  }
}

void test_shell() {
  int running = 1;
  printf("Test shell. \"help\" gives all commands.\n");
  while (running) {
    char buffer[200];
    read_command(buffer, 200);
    printf("\n");
    if (strncmp(builtin_help, buffer, strlen(builtin_help)) == 0) {
      do_help();
    } else if (strncmp(builtin_pwd, buffer, strlen(builtin_pwd)) == 0) {
      do_pwd();      
    } else if (strncmp(builtin_cd, buffer, strlen(builtin_cd)) == 0) {
      do_cd(buffer + strlen(builtin_cd));
    } else if (strncmp(builtin_ls, buffer, strlen(builtin_ls)) == 0) {
      do_ls();
    } else if (strncmp(builtin_cat, buffer, strlen(builtin_cat)) == 0) {
      do_cat(buffer + strlen(builtin_cat));
    } else if (strlen(buffer)) {
        printf("Bad command '%s'.\n", buffer);
      }
  }
}

int main(void) {
  printf("Hello, user World!\n");
  keymap = keymap_create("fi");
  test_shell();
  return 0xabba;
}
