#pragma once

#include <trwlang/node.hpp>
#include <trwlang/string_node.hpp>

#include <cassert>

namespace trwlang {

/**
 *  @invariant
 *    children.size() > 0
 *  @invariant
 *    bool(children[i]) == true
 */
struct inner_node : public node {

  std::vector<std::unique_ptr<node>> children;

  inner_node () {
  }

  inner_node (std::unique_ptr<node> head) {
    children.push_back(std::move(head));
  }

  inner_node (std::string name)
  : inner_node(make_string_node(std::move(name))) {
  }

  node_kind kind () const noexcept override {
    return node_kind::inner_node;
  }

  void print (std::ostream & ost) const override {
    children[0]->print(ost);
    ost << "[";
    if (children.size() > 1) {
      children[1]->print(ost);
      for (int i = 2; i < children.size(); ++i) {
        ost << ", ";
        children[i]->print(ost);
      }
    }
    ost << "]";
  }

  std::unique_ptr<node> clone () const override {
    auto e = std::unique_ptr<inner_node>(new inner_node());
    for (auto const & c : children) {
      e->children.push_back(c->clone());
    }
    return std::move(e);
  }

  inner_node & add_children (std::unique_ptr<node> n) & {
    assert(n);
    children.push_back(std::move(n));
    return *this;
  }

  inner_node && add_children (std::unique_ptr<node> n) && {
    assert(n);
    children.push_back(std::move(n));
    return std::move(*this);
  }

};

inner_node & node::get_inner_node () noexcept {
  return static_cast<inner_node &>(*this);
}

inner_node const & node::get_inner_node () const noexcept {
  return static_cast<inner_node const &>(*this);
}


template <class ... Args>
std::unique_ptr<inner_node> make_inner_node (Args && ... args) {
  return std::unique_ptr<inner_node>(new inner_node(std::forward<Args>(args) ...));
}

bool check_head (inner_node const & e, char const * name) {
  if (!e.children[0]->is_string_node()) {
    return false;
  }
  
  string_node const & s = static_cast<string_node const &>(*(e.children[0]));
  return s.value == name;
}

}
