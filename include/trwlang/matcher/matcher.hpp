#pragma once

#include <trwlang/node.hpp>

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

    while (stack_pattern_.size() > 0) {
      node const * p = stack_pattern_.back();
      stack_pattern_.pop_back();
      if (p == nullptr) {
        if (stack_expr_.size() == 0) {
          return false;
        }
        if (stack_expr_.back() != nullptr) {
          return false;
        }
        stack_expr_.pop_back();
        continue;
      }
      if (!consume(*p)) {
        return false;
      }
    }

    return true;
  }

private:

  bool consume (node const & p) {
    switch (p.kind()) {
    case node_kind::inner_node :
      return consume_inner_node(p.get_inner_node());
    case node_kind::string_node :
      return consume_string_node(p.get_string_node());
    case node_kind::int_node :
      return consume_int_node(p.get_int_node());
    case node_kind::bool_node :
      return consume_bool_node(p.get_bool_node());
    default :
      assert(false && "not implemented");
    }
  }

  bool consume_inner_node (inner_node const & p) {
    if (check_head(p, "PatternAny")) {
      return consume_pattern_any_node(p);
    }

    if (check_head(p, "PatternHold")) {
      return consume_pattern_hold_node(p);
    }

    if (check_head(p, "PatternOneOrMore")) {
      return consume_pattern_one_or_more_node(p);
    }

    if (stack_expr_.size() == 0) {
      return false;
    }

    if (!stack_expr_.back()->is_inner_node()) {
      return false;
    }

    inner_node const & e = stack_expr_.back()->get_inner_node();
    stack_expr_.pop_back();

    stack_pattern_.push_back(nullptr);
    for (auto i = p.size_child() - 1; i >= 0; --i) {
      stack_pattern_.push_back(&p.child(i));
    }

    stack_expr_.push_back(nullptr);
    for (auto i = e.size_child() - 1; i >= 0; --i) {
      stack_expr_.push_back(&e.child(i));
    }
    return true;
  }

  bool consume_pattern_any_node (inner_node const & p) {
    assert(check_head(p, "PatternAny"));
    if (stack_expr_.size() == 0) { return false; }
    stack_expr_.pop_back();
    return true;
  }

  bool consume_pattern_hold_node (inner_node const & p) {
    assert(check_head(p, "PatternHold"));
    if (p.size_child() != 2) {
      // TODO: error?
      return false;
    }

    auto & tag = p.child(1);
    if (!tag.is_string_node()) {
      // TODO: error?
      return false;
    }

    hold[tag.get_string_node().value] = (stack_expr_.size() > 0 ? stack_expr_.back() : nullptr);
    stack_pattern_.push_back(&p.child(2));
    return true;
  }

  bool consume_pattern_one_or_more_node (inner_node const & p) {
    assert(check_head(p, "PatternOneOrMore"));

    if (p.size_child() != 1) {
      return false;
    }

    if (stack_expr_.size() == 0) {
      return false;
    }
    if (stack_expr_.back() == nullptr) {
      return false;
    }

    stack_expr_.pop_back();
    while (true) {
      if (stack_expr_.size() == 0) {
        break;
      }
      if (stack_expr_.back() == nullptr) {
        break;
      }
      stack_expr_.pop_back();
    }
    return true;
  }

  bool consume_string_node (string_node const & p) {
    if (stack_expr_.size() == 0) {
      return false;
    }
    if (stack_expr_.back() == nullptr) {
      return false;
    }
    if (!stack_expr_.back()->is_string_node()) {
      return false;
    }
    if (stack_expr_.back()->get_string_node().value != p.value) {
      return false;
    }
    stack_expr_.pop_back();
    return true;
  }

  bool consume_int_node (int_node const & p) {
    if (stack_expr_.size() == 0) {
      return false;
    }
    if (stack_expr_.back() == nullptr) {
      return false;
    }
    if (!stack_expr_.back()->is_int_node()) {
      return false;
    }
    if (stack_expr_.back()->get_int_node().value != p.value) {
      return false;
    }
    stack_expr_.pop_back();
    return true;
  }

  bool consume_bool_node (bool_node const & p) {
    if (stack_expr_.size() == 0) {
      return false;
    }
    if (stack_expr_.back() == nullptr) {
      return false;
    }
    if (!stack_expr_.back()->is_bool_node()) {
      return false;
    }
    if (stack_expr_.back()->get_bool_node().value != p.value) {
      return false;
    }
    stack_expr_.pop_back();
    return true;
  }

  std::vector<node const *> stack_pattern_;
  std::vector<node const *> stack_expr_;
};

}
