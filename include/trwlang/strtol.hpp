#pragma once

namespace trwlang {

template <class Iterator1, class Iterator2>
long strtol (Iterator1 && first, Iterator2 last) {
  if (first == last) { return 0; }
  int flag = 0;
  long val = 0;
  int base = 10;

  auto it = first;

  if (*first == '0') {
    ++first;
    if (*first == 'x' || *first == 'X') {
      base = 16;
      ++first;
    } else {
      first = it;
      // prohibit 8 base.
      return val;
    }
  }

  if (base == 10) {
    while (first != last) {
      char ch = *first;
      if ('0' <= ch && ch <= '9') {
        val = 10 * val + (ch - '0');
        ++first;
        continue;
      }
      return val;
    }
  } else if (base == 16) {
    while (first != last) {
      char ch = *first;
      if ('0' <= ch && ch <= '9') {
        val = 16 * val + (ch - '0');
        ++first;
        continue;
      }
      if ('a' <= ch && ch <= 'f') {
        val = 16 * val + (ch - 'a' + 10);
        ++first;
        continue;
      }
      if ('A' <= ch && ch <= 'F') {
        val = 16 * val + (ch - 'A' + 10);
        ++first;
        continue;
      }
      return val;
    }
  }
  return val;
}

}
