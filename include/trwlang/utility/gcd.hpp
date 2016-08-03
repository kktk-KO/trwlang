#pragma once

#include <utility>

namespace trwlang {

long gcd (long a, long b) noexcept {
  if (b > a) { std::swap(a, b); }
  while (b != 0) {
    long r = a % b;
    a = b;
    b = r;
  }
  return a;
}

}
