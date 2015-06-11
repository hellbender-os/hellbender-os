#include <fnmatch.h>
#include <stdbool.h>

// returns zero (if matched),
// or FNM_NOMATCH (if matching failed).
static int asterisk(const char *pattern, const char *string, int flags, bool str_leading) {
  for (; *string; ++string) {
    char s = *string;
    if (*pattern && !fnmatch(pattern, string, flags)) return 0;
    // FNM_PERIOD: leading periods are matched only by an actual period.
    if (str_leading && s == '.' && flags & FNM_PERIOD) {
      return FNM_NOMATCH;
    }
    // FNM_PATHNAME: a slash are matched only by an actual slash.
    if (s == '/' && flags & FNM_PATHNAME) {
      return FNM_NOMATCH;
    }
  }
  if (!*pattern && !*string) return 0;
  else return FNM_NOMATCH;
}


// returns length of consumed pattern (if matched),
// or zero (if matching failed),
// or negative (not a proper bracket expression).
static int bracket(const char *pattern, char s, int flags) {
  const char *start = pattern;
  bool matched = false;
  int not_first = 0;
  bool negate = *pattern == '!';
  bool pat_escaped = false;
  if (negate) ++pattern;
  for (; *pattern; ++pattern, ++not_first) {
    char p = *pattern;
    if (!pat_escaped && p == '\\' && !(flags & FNM_NOESCAPE)) {
      pat_escaped = true;
      continue;
    }
    // closing bracket ends the expression:
    if (!pat_escaped && p == ']' && not_first) {
      return matched ? pattern - start : 0;
    }
    pat_escaped = false;
    // a-z gives a range:
    if (pattern[1] == '-' && pattern[2] && pattern[2] != ']') {
      if (p <= s && s <= pattern[2]) {
        matched = true;
      }
      pattern += 2; // consume the range.
      continue;
    }
    // a character in the expression:
    if (p == s) {
      matched = true;
    }
  }
  return -1;
}

int fnmatch(const char *pattern, const char *string, int flags) {
  bool pat_escaped = false;
  bool str_leading = true;
  for (; *pattern && *string; ++pattern) {
    char p = *pattern;
    char s = *string;
    
    // FNM_PERIOD: leading periods are matched only by an actual period.
    if (str_leading && s == '.' && flags & FNM_PERIOD) {
      if (p != '.') return FNM_NOMATCH;
      ++string;
      continue;
    }
    
    // FNM_PATHNAME: a slash are matched only by an actual slash.
    if (s == '/' && flags & FNM_PATHNAME) {
      if (p != '/') return FNM_NOMATCH;
      str_leading = true;
      ++string;
      continue;
    } else str_leading = false;

    // '?' matches any char (FNM_PATHNAME, FNM_PERIOD already handled)
    if (!pat_escaped && p == '?') {
      ++string;
      continue;
    }
    
    // backslash: take the next char literally, unless FNM_NOESCAPE
    if (!pat_escaped && p == '\\' && !(flags & FNM_NOESCAPE)) {
      pat_escaped = true;
      continue;
    }

    // bracket expression, or just the bracket
    if (!pat_escaped && p == '[') {
      int i = bracket(pattern, s, flags);
      if (!i) return FNM_NOMATCH;
      else if (i > 0) {
        pattern += i;
        ++string;
        continue;
      }
    }

    // asterisk matches any sequence
    if (!pat_escaped && p == '*') {
      int i = asterisk(pattern + 1, string, flags, str_leading);
      if (!i) return 0;
      else return FNM_NOMATCH;
    }

    // matches the character
    if (p != s) return FNM_NOMATCH;
    ++string;
    pat_escaped = false;
  }

  if (!*string && !*pattern) return 0;
  else return FNM_NOMATCH;
}
