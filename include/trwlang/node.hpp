#pragma once

#include <iostream>
#include <memory>

namespace trwlang {

struct node_ptr;

struct node;
struct inner_node;
struct leaf_node;
struct string_node;
struct int_node;

enum class node_kind {
  inner_node,
  string_node,
  int_node
};

struct node {

  node () noexcept = default;

  virtual node_kind kind () const noexcept = 0;

  bool is_inner_node () const noexcept {
    return kind() == node_kind::inner_node;
  }

  bool is_leaf_node () const noexcept {
    return kind() != node_kind::inner_node;
  }

  bool is_string_node () const noexcept {
    return kind() == node_kind::string_node;
  }

  bool is_int_node () const noexcept {
    return kind() == node_kind::int_node;
  }

  inner_node & get_inner_node () noexcept;
  inner_node const & get_inner_node () const noexcept;

  leaf_node & get_leaf_node () noexcept;
  leaf_node const & get_leaf_node () const noexcept;

  string_node & get_string_node () noexcept;
  string_node const & get_string_node () const noexcept;

  int_node & get_int_node () noexcept;
  int_node const & get_int_node () const noexcept;

  virtual void print (std::ostream & ost) const = 0;
  virtual std::unique_ptr<node> clone () const = 0;

  virtual ~node () noexcept = default;
};

}
