#pragma once

#include <trwlang/node.hpp>
#include <trwlang/utility/gcd.hpp>

namespace trwlang {

struct evaluater {
  std::unique_ptr<node> evaluate (node const & e) {
    if (e.is_leaf_node()) {
      return nullptr;
    }

    inner_node const & p = e.get_inner_node();
    if (!p.children[0]->is_string_node()) {
      return nullptr;
    }

    // TODO: separate evaluater
    std::string const & name = p.children[0]->get_string_node().value;

    // TODO: dispatch table

    // unary op
    if (p.children.size() == 2) {
      // TODO: dispatch table
      if (name == "Add") {
        return evaluate_add(*(p.children[1]));
      }

      if (name == "Mul") {
        return evaluate_mul(*(p.children[1]));
      }

      if (name == "Div") {
        return evaluate_div(*(p.children[1]));
      }
    }

    // binary op
    if (p.children.size() == 3) {
      if (name == "Add") {
        return evaluate_add(*(p.children[1]), *(p.children[2]));
      }

      if (name == "Mul") {
        return evaluate_mul(*(p.children[1]), *(p.children[2]));
      }

      if (name == "Div") {
        return evaluate_div(*(p.children[1]), *(p.children[2]));
      }
    }
    return nullptr;

  }

  std::unique_ptr<node> evaluate_add (node const & a1) {
    return a1.clone();
  }

  std::unique_ptr<node> evaluate_add (node const & a1, node const & a2) {
    if (!a1.is_int_node() || !a2.is_int_node()) {
      return nullptr;
    }
    return make_int_node(a1.get_int_node().value + a2.get_int_node().value);
  }

  std::unique_ptr<node> evaluate_mul (node const & a1) {
    return a1.clone();
  }

  std::unique_ptr<node> evaluate_mul (node const & a1, node const & a2) {
    if (!a1.is_int_node() || !a2.is_int_node()) {
      return nullptr;
    }
    return make_int_node(a1.get_int_node().value * a2.get_int_node().value);
  }

  std::unique_ptr<node> evaluate_div (node const & a1) {
    return a1.clone();
  }

  std::unique_ptr<node> evaluate_div (node const & a1, node const & a2) {
    if (!a1.is_int_node() || !a2.is_int_node()) {
      return nullptr;
    }

    long i = a1.get_int_node().value;
    long j = a2.get_int_node().value;

    // divide by 0.
    if (j == 0) {
      return nullptr;
    }

    // divide by 1.
    if (j == 1) {
      return make_int_node(i);
    }
 
    long r = gcd(i, j);

    // irreducible.
    if (r == 1) {
      return nullptr;
    }

    i /= r;
    j /= r;

    if (j == 1) {
      return make_int_node(i);
    }

    auto e = make_inner_node("Div");
    e->add_children(make_int_node(i));
    e->add_children(make_int_node(j));
    return std::move(e);
  }
};

}
