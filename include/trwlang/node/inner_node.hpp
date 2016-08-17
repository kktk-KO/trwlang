#pragma once

#include <trwlang/node/node.hpp>
#include <trwlang/node/string_node.hpp>

#include <cassert>

namespace trwlang {

/**
 *  @invariant
 *    children.size_child() > 0
 *  @invariant
 *    bool(children[i]) == true
 */
struct inner_node : public node {

  inner_node () {
  }

  inner_node (std::unique_ptr<node> head) {
    children_.push_back(std::move(head));
  }

  inner_node (std::string name)
  : inner_node(make_string_node(std::move(name))) {
  }

  node_kind kind () const noexcept override {
    return node_kind::inner_node;
  }

  void print (std::ostream & ost) const override {
    child(0).print(ost);
    ost << "[";
    if (size_child() > 1) {
      child(1).print(ost);
      for (int i = 2; i < size_child(); ++i) {
        ost << ", ";
        child(i).print(ost);
      }
    }
    ost << "]";
  }

  std::unique_ptr<node> clone () const override {
    auto e = std::unique_ptr<inner_node>(new inner_node());
    for (auto const & c : children_) {
      e->add_child(c->clone());
    }
    return std::move(e);
  }

  size_type size_child () const noexcept {
    return children_.size();
  }

  void add_child (std::unique_ptr<node> n) {
    assert(n);
    children_.push_back(std::move(n));
  }

  void replace_child (size_type i, std::unique_ptr<node> n) {
    assert(0 <= i && i < size_child());
    assert(n);
    children_[i] = std::move(n);
  }

  node & child(size_type i) noexcept {
    assert(0 <= i && i < size_child());
    return *children_[i];
  }

  node const & child(size_type i) const noexcept {
    assert(0 <= i && i < size_child());
    return *children_[i];
  }

private:
  std::vector<std::unique_ptr<node>> children_;
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
  if (!e.child(0).is_string_node()) {
    return false;
  }

  string_node const & s = static_cast<string_node const &>(e.child(0));
  return s.value == name;
}

}
