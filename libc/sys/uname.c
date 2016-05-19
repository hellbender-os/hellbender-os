#include <sys/utsname.h>
#include <string.h>

int uname(struct utsname *un) {
  strcpy(un->sysname, "Hellbender");
  strcpy(un->nodename, "localhost");
  strcpy(un->release, "0.1.0");
  strcpy(un->version, __DATE__ " " __TIME__);
  strcpy(un->machine, "x86_64");
  return 0;
}

