#pragma once

#include <trwlang/node/node.hpp>

namespace trwlang {

struct leaf_node : public node {

  bool is_leaf_node () const noexcept {
    return true;
  }

};

leaf_node & node::get_leaf_node () noexcept {
  return static_cast<leaf_node &>(*this);
}

leaf_node const & node::get_leaf_node () const noexcept {
  return static_cast<leaf_node const &>(*this);
}

}
