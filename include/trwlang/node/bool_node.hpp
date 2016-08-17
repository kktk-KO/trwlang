#pragma once

#include <trwlang/node/leaf_node.hpp>

#include <memory>
#include <string>
#include <vector>

namespace trwlang {

struct bool_node : public leaf_node {

  long value;

  bool_node () {
  }

  template <class ... Args>
  bool_node (Args && ... args)
  : value (std::forward<Args>(args) ...) {
  }

  node_kind kind () const noexcept override {
    return node_kind::bool_node;
  }

  void print (std::ostream & ost) const override {
    ost << (value ? "True" : "False");
  }

  std::unique_ptr<node> clone () const override {
    return std::unique_ptr<bool_node>(new bool_node(value));
  }

};

bool_node & node::get_bool_node () noexcept {
  return static_cast<bool_node &>(*this);
}

bool_node const & node::get_bool_node () const noexcept {
  return static_cast<bool_node const &>(*this);
}

template <class ... Args>
std::unique_ptr<bool_node> make_bool_node (Args && ... args) {
  return std::unique_ptr<bool_node>(new bool_node(std::forward<Args>(args) ...));
}

}
