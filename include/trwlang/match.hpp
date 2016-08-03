#pragma once

#include <trwlang/expr.hpp>
#include <trwlang/inner_node.hpp>
#include <trwlang/leaf_node.hpp>
#include <trwlang/string_node.hpp>
#include <trwlang/int_node.hpp>

#include <vector>
#include <unordered_map>

namespace trwlang {

struct matcher {
  std::unordered_map<std::string, node const *> hold;

  bool match (node const & p, node const & e) {

    stack_expr_.clear();
    stack_pattern_.clear();
    stack_expr_.push_back(&e);
    stack_pattern_.push_back(&p);

    while (stack_expr_.size() > 0) {
      if (stack_pattern_.size() == 0) {
        return false;
      }

      node const * e = stack_expr_.back();
      node const * p = stack_pattern_.back();
      stack_expr_.pop_back();
      stack_pattern_.pop_back();

      if (!consume(p, e)) {

        return false;
      }
    }
    while (stack_pattern_.size() > 0) {
      node const * p = stack_pattern_.back();
      stack_pattern_.pop_back();
      if (!consume(p, nullptr)) {
        return false;
      }
    }
    return true;
  }

private:

  bool consume (node const * p, node const * e) {
    if (p == nullptr) { return false; }
    switch (p->kind()) {
    case node_kind::inner_node :
      return consume_inner_node(p->get_inner_node(), e);
    case node_kind::string_node :
      return consume_string_node(p->get_string_node(), e);
    case node_kind::int_node :
      return consume_int_node(p->get_int_node(), e);
    default :
      assert(false && "not implemented");
    }
  }

  bool consume_inner_node (inner_node const & p, node const * e) {
    if (!p.head) { return false; }

    if (check_head(p, "PatternAny")) {
      return e != nullptr;
    }

    if (check_head(p, "PatternHold")) {
      if (p.children.size() != 2) {
        return false;
      }

      if (!p.children[0]->is_string_node()) {
        return false;
      }

      hold[p.children[0]->get_string_node().value] = e;
      stack_pattern_.push_back(p.children[1].get());
      stack_expr_.push_back(e);
      return e != nullptr;
    }

    if (e == nullptr) {
      return false;
    }
    if (!e->is_inner_node()) {
      return false;
    }

    inner_node const & ei = e->get_inner_node();

    for (auto first = p.children.rbegin(), last = p.children.rend(); first != last; ++first) {
      stack_pattern_.push_back(first->get());
    }
    stack_pattern_.push_back(p.head.get());
    for (auto first = ei.children.rbegin(), last = ei.children.rend(); first != last; ++first) {
      stack_expr_.push_back(first->get());
    }
    stack_expr_.push_back(ei.head.get());
    return true;
  }

  bool consume_string_node (string_node const & p, node const * e) {
    if (e == nullptr) { return false; }
    if (!e->is_string_node()) { return false; }
    return p.value == e->get_string_node().value;
  }

  bool consume_int_node (int_node const & p, node const * e) {
    if (e == nullptr) { return false; }
    if (!e->is_int_node()) { return false; }
    return p.value == e->get_int_node().value;
  }

  std::vector<node const *> stack_pattern_;
  std::vector<node const *> stack_expr_;
};

}
