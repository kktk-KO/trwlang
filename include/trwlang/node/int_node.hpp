#pragma once

#include <trwlang/node/leaf_node.hpp>

#include <memory>
#include <string>
#include <vector>

namespace trwlang {

struct int_node : public leaf_node {

  long value;

  int_node () {
  }

  template <class ... Args>
  int_node (Args && ... args)
  : value (std::forward<Args>(args) ...) {
  }

  node_kind kind () const noexcept override {
    return node_kind::int_node;
  }

  void print (std::ostream & ost) const override {
    ost << value;
  }

  std::unique_ptr<node> clone () const override {
    return std::unique_ptr<int_node>(new int_node(value));
  }

};

int_node & node::get_int_node () noexcept {
  return static_cast<int_node &>(*this);
}

int_node const & node::get_int_node () const noexcept {
  return static_cast<int_node const &>(*this);
}

template <class ... Args>
std::unique_ptr<int_node> make_int_node (Args && ... args) {
  return std::unique_ptr<int_node>(new int_node(std::forward<Args>(args) ...));
}

}
