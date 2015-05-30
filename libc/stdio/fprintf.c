#include <stdbool.h>
#include <wchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hellbender.h>

#if defined(__is_hellbender_kernel)

size_t _fprint_n(FILE *file, const char* data, size_t data_length) {
  (void)(file);
  for (size_t i = 0; i < data_length; i++) {
    putchar((int) ((const unsigned char*) data)[i]);
  }
  return data_length;
}

#else

size_t _fprint_n(FILE *file, const char* data, size_t data_length) {
  if (file == NULL) { // assume libC is not yet set up.
    syscall_print(data, data_length);

  } else if (file->buffer == NULL && file->fd >= 0) { // unbuffered
    write(file->fd, data, data_length);
    
  } else { // buffered
    for (size_t i = 0; i < data_length; i++) {
      if (file->buf_bytes == file->buf_size) fflush(file);
      if (file->buf_bytes < file->buf_size)
        file->buffer[(file->buf_bytes)++] = data[i];
      if (data[i] == '\n') fflush(file);
    }
  }
  return data_length;
}

#endif

int fprintf(FILE *file, const char* format, ...)
{
  va_list parameters;
  va_start(parameters, format);
  int bytes = vfprintf(file, format, parameters);
  va_end(parameters);
  return bytes;
}

struct spec {
  bool group_separator;
  bool left_justified;
  bool plus_sign;
  bool space_sign;
  bool pad_zero;
  bool alt_form;
  bool use_precision;
  bool minwidth_asterisk;
  bool precision_asterisk;
  int minwidth_value;
  int precision_value;
  int value_idx;
  int precision_idx;
  int minwidth_idx;
  int len_h;
  int len_l;
  int len_j;
  int len_z;
  int len_t;
  int len_L;
  int specifier;
  const char *start_ptr;
  const char *end_ptr;
  wchar_t tmp[2];
};

union value {
    intmax_t i;
    uintmax_t u;
    long double d;
    unsigned char c;
    char *s;
    wchar_t *ws;
    int *n_i;
    char *n_c;
    short *n_s;
};

static int parse_specifiers(struct spec *specs,
                            size_t *spec_order,
                            const char* format) {
  const char spec_str[] = "diouxXfFeEgGaAcspnCS%";
  int n_spec = 0;
  int next_idx = 1;
  struct spec mock_spec = {0};
  mock_spec.specifier = 'i'; // * arguments are integers.

  while (*format) {
    // scan and output until next specifier, or end of format.
    while (*format && *format != '%') format++;
    if (!*format) break;
    
    // start analyzing format specifier:
    unsigned number = 0;
    bool first_digit = 1;
    bool has_number = 0;
    void *spec_ptr = NULL;
    struct spec spec = {0};
    spec.precision_value = 1;
    spec.start_ptr = format++;
    
    while (*format && !spec.specifier) {
      char c = *format++;
      // check flags, min width, precision:
      switch (c) {
      case '\'': spec.group_separator = 1; break;
      case '-':  spec.left_justified  = 1; break;
      case '+':  spec.plus_sign       = 1; break;
      case ' ':  spec.space_sign      = 1; break;
      case '#':  spec.alt_form        = 1; break;
      case '0':  if (first_digit) spec.pad_zero = 1;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        first_digit = 0;
        has_number = 1;
        number = number * 10 + c - '0';
        break;
      case '.':
        first_digit = 0;
        if (has_number) spec.minwidth_value = number;
        has_number = 0;
        number = 0;
        spec.use_precision = 1;
        break;
      case '*':
        if (spec.use_precision) spec.precision_asterisk = 1;
        else spec.minwidth_asterisk = 1;
        break;
      case '$':
        if (!spec.value_idx) spec.value_idx = number;
        else if (spec.use_precision) spec.precision_idx = number;
        else spec.minwidth_idx = number;
        has_number = 0;
        number = 0;
        break;
      default:
        if (has_number) {
          if (spec.use_precision) spec.precision_value = number;
          else spec.minwidth_value = number;
          has_number = 0;
          number = 0;
        }
        // check length modifiers, conversion specifiers:
        switch (c) {
        case 'h': spec.len_h++; break;
        case 'l': spec.len_l++; break;
        case 'j': spec.len_j++; break;
        case 'z': spec.len_z++; break;
        case 't': spec.len_t++; break;
        case 'L': spec.len_L++; break;
        default:
          if (n_spec == NL_ARGMAX) return -1;
          spec_ptr = strchr(spec_str, c);
          if (spec_ptr) spec.specifier = c;
          else return -1;
          if (spec.minwidth_asterisk && !spec.minwidth_idx)
            spec.minwidth_idx = next_idx++;
          if (spec.precision_asterisk && !spec.precision_idx)
            spec.precision_idx = next_idx++;
          if (!spec.value_idx) spec.value_idx = next_idx++;
          spec.end_ptr = format;
          spec_order[spec.value_idx] = ++n_spec;
          specs[n_spec] = spec;
          if (spec.minwidth_idx) specs[spec.minwidth_idx] = mock_spec;
          if (spec.precision_idx) specs[spec.minwidth_idx] = mock_spec;
          break;
        }
      }
    }
  }

  mock_spec.start_ptr = format;
  mock_spec.specifier = ';';
  specs[++n_spec] = mock_spec;
  return n_spec;
}

static int read_parameters(struct spec *specs,
                           size_t *spec_order,
                           int n_specs,
                           union value *values,
                           va_list parameters) {
  for (int i = 1; i < n_specs; ++i) { // last one is the end-of-format marker.
    size_t j = spec_order[i];
    switch (specs[j].specifier) {
    case 'd': // signed integer types:
    case 'i':
      if (specs[j].len_j)
        { values[i].i = va_arg(parameters, intmax_t); break; }
      if (specs[j].len_z)
        { values[i].i = va_arg(parameters, ssize_t); break; }
      if (specs[j].len_t)
        { values[i].i = va_arg(parameters, ptrdiff_t); break; }
      if (specs[j].len_h == 2)
        { values[i].i = (char)va_arg(parameters, int); break; }
      if (specs[j].len_h == 1)
        { values[i].i = (short)va_arg(parameters, int); break; }
      switch (specs[j].len_l) {
      case 0: values[i].i = va_arg(parameters, int); break;
      case 1: values[i].i = va_arg(parameters, long); break;
      case 2: values[i].i = va_arg(parameters, long long); break;
      default: return -1;
      }
      break;
    case 'o': // unsigned integer types:
    case 'u':
    case 'x':
    case 'X':
      if (specs[j].len_j)
        { values[i].u = va_arg(parameters, uintmax_t); break; }
      if (specs[j].len_z)
        { values[i].u = va_arg(parameters, size_t); break; }
      if (specs[j].len_t) // TODO: WTF is "unsigned ptrdiff_t?"
        { values[i].u = va_arg(parameters, ptrdiff_t); break; }
      if (specs[j].len_h == 2)
        { values[i].i = (unsigned char)va_arg(parameters, unsigned); break; }
      if (specs[j].len_h == 1)
        { values[i].i = (unsigned short)va_arg(parameters, unsigned); break; }
      switch (specs[j].len_l) {
      case 0: values[i].u = va_arg(parameters, unsigned); break;
      case 1: values[i].u = va_arg(parameters, unsigned long); break;
      case 2: values[i].u = va_arg(parameters, unsigned long long); break;
      default: return -1;
      }
      break;
    case 'f': // floating point types:
    case 'F':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
    case 'a':
    case 'A':
      if (specs[j].len_L) values[i].d = va_arg(parameters, long double);
      else values[i].d = va_arg(parameters, double);
      break;
    case 'C': // character types:
      specs[j].len_l++;
      specs[j].specifier = 'c';
      // fall through
    case 'c':
      if (specs[j].len_l) {
        wint_t w = va_arg(parameters, wint_t);
        specs[j].tmp[0] = (wchar_t)w;
        values[i].ws = specs[j].tmp;
        specs[j].specifier = 's';
      } else {
        values[i].c = (unsigned char)va_arg(parameters, int);
      }
      break;
    case 'S': // string types:
      specs[j].len_l++;
      specs[j].specifier = 's';
      // fall through
    case 's':
      if (specs[j].len_l) values[i].ws = va_arg(parameters, wchar_t*);
      else values[i].s = va_arg(parameters, char*);
      break;
    case 'p': // pointer type:
      values[i].u = (uintptr_t)va_arg(parameters, void*);
      specs[j].specifier = 'x';
      break;
    case 'n': // 'number of chars written so far' type:
      switch (specs[j].len_h) {
      case 2: values[i].n_c = va_arg(parameters, char*); break;
      case 1: values[i].n_s = va_arg(parameters, short*); break;
      case 0: values[i].n_i = va_arg(parameters, int*); break;
      default: return -1;
      }
      break;
    case '%': break; // nothing to pop.
    default: return -1;
    }
  }
  return n_specs;
}

static inline char* pad_sign(struct spec* spec, char* ptr) {
  if (*ptr != '-') {
    if (*ptr && spec->plus_sign) {
      *(--ptr) = '+';
    } else if (spec->space_sign) {
      *(--ptr) = ' ';
    }
  }
  return ptr;
}

static inline char* pad_zeros(int precision, char* ptr, char* end) {
  char *limit = end - precision;
  char sign = 0; // sign character must be moved when we pad with zeroes.
  if (*ptr == '+' || *ptr == '-' || *ptr == ' ') {
    sign = *ptr;
    ++ptr;
  }
  while (ptr > limit) {
    *(--ptr) = '0';
  }
  if (sign) *(--ptr) = sign;
  return ptr;
}

static inline char* pad_left(struct spec* spec, char *ptr, char *end) {
  if (spec->left_justified) return ptr;
  int len = end - ptr;
  if (spec->pad_zero) {
    char sign = 0; // sign character must be moved when we pad with zeroes.
    if (*ptr == '+' || *ptr == '-' || *ptr == ' ') {
      sign = *ptr;
      ++ptr;
    }
    for (int i = len; i < spec->minwidth_value; ++i) {
      *(--ptr) = '0';
    }
    if (sign) *(--ptr) = sign;
    
  } else {
    for (int i = len; i < spec->minwidth_value; ++i) {
      *(--ptr) = ' ';
    }
  }
  return ptr;
}

static inline size_t pad_right(FILE *file, struct spec* spec, size_t len) {
  if (!spec->left_justified) return 0;
  int n = 0;
  for (int i = (int)len; i < spec->minwidth_value; ++i) {
    fputc(' ', file);
    ++n;
  }
  return n;
}

static inline char* do_alt_form(int base, char* ptr) {
  if (base == 8) {
    if (*ptr != '0') *(--ptr) = '0';
  }
  if (base == 16 && *ptr != '0') {
    *(--ptr) = 'x';
    *(--ptr) = '0';
  }
  if (base == -16 && *ptr != '0') {
    *(--ptr) = 'X';
    *(--ptr) = '0';
  }
  return ptr;
}

static const char precent = '%';

#define TMP_SIZE 128
int vfprintf(FILE* file, const char* format, va_list parameters) {
  int written = 0;
  char tmp[TMP_SIZE];
  char *end = tmp + TMP_SIZE - 1;
  char *ptr;
  int base = 0;
  size_t len;

  struct spec specs[NL_ARGMAX+2]; // specs are numbered from one + EOF marker.
  size_t order[NL_ARGMAX+1]; // EOF not included here.
  union value values[NL_ARGMAX+1];

  int n_specs = parse_specifiers(specs, order, format);
  if (n_specs == -1) return -1;
  if (read_parameters(specs, order, n_specs, values, parameters) == -1)
    return -1;
  
  // output everything:
  for (int i = 1; i <= n_specs; ++i) {
    if (specs[i].start_ptr) { // this will skip those * arguments.
      // print stuff before the next specifier:
      len = specs[i].start_ptr - format;
      written += _fprint_n(file, format, len);
      format = specs[i].end_ptr;

      // get minwidth and precision, if they came from arguments:
      if (specs[i].minwidth_asterisk) {
        specs[i].minwidth_value = values[specs[i].minwidth_idx].i;
      }
      if (specs[i].precision_asterisk) {
        specs[i].precision_value = values[specs[i].precision_idx].i;
      }

      // format the specifier:
      // TODO: this does not support apostrophe (grouping characters).
      switch (specs[i].specifier) {
      case ';': // end of format marker.
        break;
      case '%':
        fputc('%', file);
        break;
      case 'd':
      case 'i':
        ptr = _lltoa_n(values[specs[i].value_idx].i, tmp, TMP_SIZE, 10);
        if (*ptr == '0' && !specs[i].precision_value) ++ptr;
        else if (specs[i].use_precision) {
          ptr = pad_zeros(specs[i].precision_value, ptr, end);
          specs[i].pad_zero = 0;
        }
        if (specs[i].alt_form) {
          ptr = do_alt_form(base, ptr);
          specs[i].pad_zero = 0;
        }
        ptr = pad_sign(&specs[i], ptr);
        ptr = pad_left(&specs[i], ptr, end);
        len = end - ptr;
        written += _fprint_n(file, ptr, len);
        written += pad_right(file, &specs[i], len);
        break;
      case 'o': base = 8; goto print_u;
      case 'u': base = 10; goto print_u;
      case 'x': base = 16; goto print_u;
      case 'X': base = -16; goto print_u;
      print_u:
        ptr = _ulltoa_n(values[specs[i].value_idx].u, tmp, TMP_SIZE, base);
        if (*ptr == '0' && !specs[i].precision_value) ++ptr;
        else if (specs[i].use_precision) {
          ptr = pad_zeros(specs[i].precision_value, ptr, end);
          specs[i].pad_zero = 0;
        }
        if (specs[i].alt_form) {
          ptr = do_alt_form(base, ptr);
          specs[i].pad_zero = 0;
        }
        ptr = pad_left(&specs[i], ptr, end);
        len = end - ptr;
        written += _fprint_n(file, ptr, len);
        written += pad_right(file, &specs[i], len);
        break;
      case 'c':
        fputc((int)values[specs[i].value_idx].c, file);
        ++written;
        break;
      case 's':
        // TODO: support ls
        len = strlen(values[specs[i].value_idx].s);
        _fprint_n(file, values[specs[i].value_idx].s, len);
        break;
      default:
        // TODO: fFeEgGaApn
        len = specs[i].end_ptr - specs[i].start_ptr;
        _fprint_n(file, specs[i].start_ptr, len);
      }
    }
  }
  return written;
}
