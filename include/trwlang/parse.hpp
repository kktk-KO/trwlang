#pragma once

#include <trwlang/node.hpp>
#include <trwlang/utility/strtol.hpp>

namespace trwlang {

template <class Iterator>
bool parse_ascii_ (Iterator & first, Iterator const & last, char ch);

template <class Iterator>
bool parse_ascii (Iterator & first, Iterator const & last, char ch);

template <class Iterator>
bool parse_fixed_string_ (Iterator & first, Iterator const & last, char const * str);

template <class Iterator>
bool parse_fixed_string (Iterator & first, Iterator const & last, char const * str);

template <class Iterator>
bool parse_char_ (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_char (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_digit_ (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_digit (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_alpha_ (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_alpha (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_whitespace (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_whitespaces (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_string_ (Iterator & first, Iterator const & last);

template <class Iterator>
bool parse_string (Iterator & first, Iterator const & last);

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_string_node (Iterator1 && first, Iterator2 const & last);

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_int_node (Iterator1 && first, Iterator2 const & last);

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_group_node (Iterator1 && first, Iterator2 const & last);

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_leaf_node (Iterator1 && first, Iterator2 const & last);

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_mul_div_node (Iterator1 && first, Iterator2 const & last);

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_node (Iterator1 && first, Iterator2 const & last);

template <class Iterator>
bool parse_ascii_ (Iterator & first, Iterator const & last, char ch) {
  if (first == last) { return false; }
  if (*first != ch) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_ascii (Iterator & first, Iterator const & last, char ch) {
  if (parse_ascii_(first, last, ch)) {
    parse_whitespaces(first, last);
    return true;
  }
  return false;
}

template <class Iterator>
bool parse_fixed_string_ (Iterator & first, Iterator const & last, char const * str) {
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
bool parse_fixed_string (Iterator & first, Iterator const & last, char const * str) {
  if (parse_fixed_string_(first, last, str)) {
    parse_whitespaces(first, last);
    return true;
  }
  return false;
}

template <class Iterator>
bool parse_char_ (Iterator & first, Iterator const & last) {
  if (first == last) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_char (Iterator & first, Iterator const & last) {
  if (parse_char_(first, last)) {
    parse_whitespaces(first, last);
    return true;
  }
  return false;
}

template <class Iterator>
bool parse_digit_ (Iterator & first, Iterator const & last) {
  if (first == last) { return false; }
  char ch = *first;
  if (!('0' <= ch && ch <= '9')) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_digit (Iterator & first, Iterator const & last) {
  if (parse_char_(first, last)) {
    parse_whitespaces(first, last);
    return true;
  }
  return false;
}

template <class Iterator>
bool parse_alpha_ (Iterator & first, Iterator const & last) {
  if (first == last) { return false; }
  char ch = *first;
  if (!('A' <= ch && ch <= 'Z') && !('a' <= ch && ch <= 'z')) { return false; }
  ++first;
  return true;
}

template <class Iterator>
bool parse_alpha (Iterator & first, Iterator const & last) {
  if (parse_char(first, last)) {
    parse_whitespaces(first, last);
    return true;
  }
  return false;
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
bool parse_string_ (Iterator & first, Iterator const & last) {
  if (!parse_alpha_(first, last)) { return false; }
  while (parse_alpha_(first, last) || parse_digit_(first, last)) {}
  return true;
}

template <class Iterator>
bool parse_string (Iterator & first, Iterator const & last) {
  if (parse_string_(first, last)) {
    parse_whitespaces(first, last);
    return true;
  }
  return false;
}

/**
 *  PatternAnyNode := '_'
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_pattern_any_node (Iterator1 && first, Iterator2 const & last) {
  auto it1 = first;
  if (!parse_ascii(first, last, '_')) { return nullptr; }
  return make_inner_node(make_string_node("PatternAny"));
}

/**
 *  StringNode := String | String '_'
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_string_node (Iterator1 && first, Iterator2 const & last) {
  auto it1 = first;
  if (!parse_string_(first, last)) { return nullptr; }
  auto it2 = first;
  parse_whitespaces(first, last);
  if (parse_ascii(first, last, '_')) {
    auto ret = make_inner_node("PatternHold");
    ret->add_child(make_string_node(it1, it2));
    ret->add_child(make_inner_node(make_string_node("PatternAny")));
    return std::move(ret);
  }
  return make_string_node(it1, it2);
}

/**
 *  IntNode := Int
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_int_node (Iterator1 && first, Iterator2 const & last) {
  auto it = first;
  long val = strtol(first, last);
  if (it == first) { return nullptr; }
  parse_whitespaces(first, last);
  return make_int_node(val);
}

/**
 *  BoolNode := "true" | "false"
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_bool_node (Iterator1 && first, Iterator2 const & last) {
  auto it = first;
  if (parse_fixed_string(first, last, "true")) {
    return make_bool_node(true);
  }
  if (parse_fixed_string(first, last, "false")) {
    return make_bool_node(false);
  }
  return nullptr;
}

/**
 *  GroupNode = '(' Node ')'
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_group_node (Iterator1 && first, Iterator2 const & last) {
  if (!parse_ascii(first, last, '(')) {
    return nullptr;
  }
  auto e = parse_node(first, last);
  if (!e) {
    // TODO: diagnostics
    return nullptr;
  }
  if (!parse_ascii(first, last, ')')) {
    // TODO: diagnostics
  }

  return std::move(e);
}

/**
 *  LeafNode :=
 *    BoolNode
 *    PatternAnyNode
 *    StringNode
 *    IntNode
 *    GroupNode
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_leaf_node (Iterator1 && first, Iterator2 const & last) {
  auto e = parse_bool_node(first, last);
  if (e) { return std::move(e); }
  e = parse_pattern_any_node(first, last);
  if (e) { return std::move(e); }
  e = parse_string_node(first, last);
  if (e) { return std::move(e); }
  e = parse_int_node(first, last);
  if (e) { return std::move(e); };
  e = parse_group_node(first, last);
  if (e) { return std::move(e); }
  return nullptr;
}

/**
 *  InnerNode :=
 *    LeafNode
 *    LeafNode '[' ']'
 *    LeafNode '[' Node (',' Node)* ']'
 *    LeafNode '[' Node (',' Node)* ',' ']'
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_inner_node (Iterator1 && first, Iterator2 const & last) {
  auto head = parse_leaf_node(first, last);

  if (!head) { return nullptr; }
 
  if (!parse_ascii(first, last, '[')) { return std::move(head); }
  auto e = make_inner_node(std::move(head));

  auto p = parse_node(first, last);
  if (p) {
    e->add_child(std::move(p));
    while (true) {
      if (!parse_ascii(first, last, ',')) {
        break;
      }
      p = parse_node(first, last);
      if (!p) {
        // TODO diagnostics
        return nullptr;
      }
      e->add_child(std::move(p));
    }
    parse_ascii(first, last, ',');
  }

  if (!parse_ascii(first, last, ']')) {
    // TODO  diagnostics
    return nullptr;
  }

  return std::move(e);
}

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_implicit_mul_node (Iterator1 && first, Iterator2 const & last) {

}

/**
 *  MulDivNode :=
 *    InnerNode
 *    InnerNode * InnerNode
 *    InnerNode / InnerNode
 *    MulDivNode * InnerNode
 *    MulDivNode / InnerNode
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_mul_div_node (Iterator1 && first, Iterator2 const & last) {
  auto e = parse_inner_node(first, last);
  if (!e) {
    return nullptr;
  }

  std::unique_ptr<inner_node> p;
  if (parse_ascii(first, last, '*')) {
    p = make_inner_node("Mul");
  } else if (parse_ascii(first, last, '/')) {
    p = make_inner_node("Div");
  } else {
    return std::move(e);
  }
  p->add_child(std::move(e));

  while (true) {
    auto f = parse_inner_node(first, last);
    if (!f) {
      // TODO diagnostics
      return nullptr;
    }

    std::unique_ptr<inner_node> r;
    if (parse_ascii(first, last, '*')) {
      r = make_inner_node("Mul");
    } else if (parse_ascii(first, last, '/')) {
      r = make_inner_node("Div");
    } else {
      p->add_child(std::move(f));
      break;
    }

    p->add_child(std::move(f));
    r->add_child(std::move(p));
    p = std::move(r);
  }

  return std::move(p);
}

/*
 *  AddSubNode :=
 *    MulDivNode
 *    MulDivNode + MulDivNode
 *    MulDivNode - MulDivNode
 *    AddSubNode + MulDivNode
 *    AddSubNode - MulDivNode
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_add_sub_node (Iterator1 && first, Iterator2 const & last) {
  auto e = parse_mul_div_node(first, last);
  if (!e) {
    return nullptr;
  }

  std::unique_ptr<inner_node> p;
  auto it = first;
  if (parse_ascii(first, last, '+')) {
    p = make_inner_node("Add");
  } else if (parse_ascii(first, last, '-')) {
    if (parse_ascii(first, last, '>')) {
      first = it;
      return std::move(e);
    }
    p = make_inner_node("Sub");
   } else {
    return std::move(e);
  }
  p->add_child(std::move(e));

  while (true) {
    auto f = parse_mul_div_node(first, last);
    if (!f) {
      // TODO diagnostics
      return nullptr;
    }

    std::unique_ptr<inner_node> r;
    if (parse_ascii(first, last, '+')) {
      r = make_inner_node("Add");
    } else if (parse_ascii(first, last, '-')) {
      r = make_inner_node("Sub");
    } else {
      p->add_child(std::move(f));
      break;
    }

    p->add_child(std::move(f));
    r->add_child(std::move(p));
    p = std::move(r);
  }

  return std::move(p);
}

/**
 *  RuleNode := AddSubNode ('->' AddSubNode)?
 */
template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_rule_node (Iterator1 && first, Iterator2 const & last) {
  auto e = parse_add_sub_node(first, last);

  if (!e) {
    return nullptr;
  }

  if (!parse_fixed_string(first, last, "->")) {
    return std::move(e);
  }

  auto e2 = parse_add_sub_node(first, last);
  if (!e2) { return nullptr; }

  auto ret = make_inner_node("Rule");
  ret->add_child(std::move(e));
  ret->add_child(std::move(e2));
  return std::move(ret);
}

template <class Iterator1, class Iterator2>
std::unique_ptr<node> parse_node (Iterator1 && first, Iterator2 const & last) {
  return parse_rule_node(first, last);
}

std::unique_ptr<node> parse_node (std::string const & str) {
  return parse_node(str.begin(), str.end());
}

}
