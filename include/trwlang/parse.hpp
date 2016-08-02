#pragma once

#include <trwlang/expr.hpp>
#include <trwlang/node.hpp>
#include <trwlang/inner_node.hpp>
#include <trwlang/int_node.hpp>
#include <trwlang/leaf_node.hpp>
#include <trwlang/string_node.hpp>

#include <trwlang/strtol.hpp>

namespace trwlang {

template <class Iterator>
bool parse_ascii (Iterator & first, Iterator const & last, char ch) {
  if (first == last) { return false; }
  if (*first != ch) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_str (Iterator & first, Iterator const & last, char const * str) {
  if (first == last) { return false; }
  auto it = first;

  while (first != last && *str != '\0') {
    if (*first != *str) {
      first = it;
      return false;
    }
    ++first;
    ++str;
  }
  if (*str != '\0') {
    first = it;
    return false;
  }
  return true;
}

template <class Iterator>
bool parse_char (Iterator & first, Iterator const & last) {
  if (first == last) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_digit (Iterator & first, Iterator const & last) {
  if (first == last) { return false; }
  char ch = *first;
  if (!('0' <= ch && ch <= '9')) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_alpha (Iterator & first, Iterator const & last) {
  if (first == last) { return false; }
  char ch = *first;
  if (!('A' <= ch && ch <= 'Z') && !('a' <= ch && ch <= 'z')) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_whitespace (Iterator & first, Iterator const & last) {
  if (first == last) { return false; }
  char ch = *first;
  if (!(ch == ' ') && !(ch == '\n') && !(ch == '\r') && !(ch == '\t')) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_whitespaces (Iterator & first, Iterator const & last) {
  if (!parse_whitespace(first, last)) { return false; }
  while (parse_whitespace(first, last)) {}
  return true;
}

template <class Iterator>
bool parse_string (Iterator & first, Iterator const & last) {
  if (!parse_alpha(first, last)) { return false; }
  while (parse_alpha(first, last) || parse_digit(first, last)) {}
  return true;
}

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_term (Iterator1 && first, Iterator2 const & last) {
  auto it = first;
  if (parse_string(first, last)) {
    auto it2 = first;
    if (parse_ascii(first, last, '_')) {
      parse_whitespaces(first, last);
      auto e = make_inner_node(make_string_node("PatternHold"));
      e->children.push_back(make_string_node(it, it2));
      e->children.push_back(make_inner_node(make_string_node("PatternAny")));
      return std::move(e);
    }
    return make_string_node(it, it2);
  }
  if (parse_ascii(first, last, '_')) {
    parse_whitespaces(first, last);
    return make_inner_node(make_string_node("PatternAny"));
  }

  long val = strtol(first, last);
  if (first != it) {
    return make_int_node(val);
  }
  return nullptr;
}

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_node (Iterator1 && first, Iterator2 const & last) {

  auto head = parse_term(first, last);

  if (!head) { return nullptr; }
 
  if (!parse_ascii(first, last, '[')) { return std::move(head); }
  parse_whitespaces(first, last);
  auto e = make_inner_node(std::move(head));

  while (true) {
    auto p = parse_node(first, last);
    if (!p) { break; }

    parse_ascii(first, last, ',');
    parse_whitespaces(first, last);
    e->children.push_back(std::move(p));
  }

  parse_whitespaces(first, last);
  if (!parse_ascii(first, last, ']')) { return nullptr; }
  parse_whitespaces(first, last);

  return std::move(e);
}

std::unique_ptr<node> parse_node (std::string const & str) {
  return parse_node(str.begin(), str.end());
}

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_expr (Iterator1 && first, Iterator2 const & last) {
  auto e = parse_node(first, last);
  if (!e) { return nullptr; }
  parse_whitespaces(first, last);
  if (!parse_str(first, last, "->")) {
    return std::move(e);
  }
  parse_whitespaces(first, last);
  auto e2 = parse_node(first, last);
  if (!e2) { return nullptr; }
  auto e3 = make_inner_node(make_string_node("Rule"));
  e3->children.emplace_back(std::move(e));
  e3->children.emplace_back(std::move(e2));
  return std::move(e3);
}

}
