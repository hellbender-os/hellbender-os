#include <unistd.h>
#include <string.h>

static char* _optchar;
char *optarg;
int optind;
int opterr;
int optopt;

int getopt(int argc, char * const argv[], const char *optstring) {
  optarg = NULL;
  if (!optind) {
    optind = 1;
    _optchar = NULL;
  }
  if (!_optchar || !*_optchar) {
    if (optind >= argc) return -1;
    if (!argv[optind] || *argv[optind] != '-' || !argv[optind][1]) {
      return -1;
    }
    if (argv[optind][1] == '-' && !argv[optind][2]) {
      ++optind;
      return -1;
    }
    _optchar = argv[optind] + 1;
  }
  char c = *_optchar++;
  if (!*_optchar) {
    optind++;
  }
  char *o = strchr(optstring, c);
  if (!o) {
    optopt = c;
    return '?';
    //TODO: print diagnostic to stderr unless optstring[0]==':' or opterr==0
  }
  if (o[1] == ':') {
    if (*_optchar) {
      optarg = _optchar;
      optind++;
      _optchar = NULL;
    } else {
      if (optind < argc) {
        optarg = argv[optind++];
      } else {
        optopt = c;
        if (optstring[0] == ':') return ':';
        else {
          //TODO: print diagnostic to stderr unless opterr==0
          return '?';
        }
      }
    }
  }
  return c;
}

