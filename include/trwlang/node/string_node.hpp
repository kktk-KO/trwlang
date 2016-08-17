#pragma once

#include <trwlang/node/leaf_node.hpp>

#include <memory>
#include <string>
#include <vector>

namespace trwlang {

struct string_node : public leaf_node {

  std::string value;

  string_node () {
  }

  template <class ... Args>
  string_node (Args && ... args)
  : value(std::forward<Args>(args) ...) {
  }

  node_kind kind () const noexcept override {
    return node_kind::string_node;
  }

  void print (std::ostream & ost) const override {
    ost << value;
  }

  std::unique_ptr<node> clone () const override {
    return std::unique_ptr<string_node>(new string_node(value));
  }
};

string_node & node::get_string_node () noexcept {
  return static_cast<string_node &>(*this);
}

string_node const & node::get_string_node () const noexcept {
  return static_cast<string_node const &>(*this);
}

template <class ... Args>
std::unique_ptr<string_node> make_string_node (Args && ... args) {
  return std::unique_ptr<string_node>(new string_node(std::forward<Args>(args) ...));
}

}
