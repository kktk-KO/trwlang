#pragma once

#include <trwlang/node.hpp>
#include <trwlang/inner_node.hpp>
#include <trwlang/int_node.hpp>
#include <trwlang/parse.hpp>
#include <trwlang/string_node.hpp>
#include <trwlang/match.hpp>
#include <trwlang/utility/gcd.hpp>

#include <unordered_map>

namespace trwlang {

struct context {

  using rule = std::pair<std::unique_ptr<node>, std::unique_ptr<node>>;
  std::unordered_map<std::string, std::vector<rule>> rule_named;
  std::vector<rule> rule_other;
  matcher m;

  context () {
    add_rule(parse_node("Sub[x_, y_]"), parse_node("Add[x, Mul[-1, y]]"));
    add_rule(parse_node("Div[Div[x_, y_], z_]"), parse_node("Div[x, Mul[y, z]]"));
    add_rule(parse_node("Mul[Div[x_, y_], z_]"), parse_node("Div[Mul[x, z], y]"));
  }

  void add_rule (std::unique_ptr<node> p, std::unique_ptr<node> e) {
    if (!p || !e) { return; }
    if (p->is_inner_node()) {
      auto & p2 = p->get_inner_node();
      if (p2.head->is_string_node()) {
        std::string const & name = p2.head->get_string_node().value;
        rule_named[name].emplace_back(std::move(p), std::move(e));
        return;
      }
    }
    rule_other.emplace_back(std::move(p), std::move(e));
  }

  void evaluate_all (std::unique_ptr<node> & e) {
    auto root = make_inner_node(std::move(e));

    stack_eval_.clear();
    stack_eval_.emplace_back(root.get(), 0);
    while (stack_eval_.size() > 0) {
      std::pair<inner_node *, int> p = stack_eval_.back();
      if (p.second == 0) {
        stack_eval_.back().second = 1;
        if (p.first->head->is_inner_node()) {
          stack_eval_.emplace_back(&(p.first->head->get_inner_node()), 0);
        }
        for (auto & c : p.first->children) {
          if (c->is_inner_node()) {
            stack_eval_.emplace_back(&(c->get_inner_node()), 0);
          }
        }
      } else {
        stack_eval_.pop_back();
        bool flag = false;

        auto e2 = evaluate_or_apply(*(p.first->head));
        if (e2) {
          p.first->head = std::move(e2);
          flag = true;
        }
        for (auto & c : p.first->children) {
          e2 = evaluate_or_apply(*c);
          if (e2) {
            c = std::move(e2);
            flag = true;
          }
        }
        if (flag) {
          stack_eval_.emplace_back(p.first, 0);
        }
      }
    }
    e = std::move(root->head);
  }

  void evaluate_once (std::unique_ptr<node> & e) {
    auto root = make_inner_node(std::move(e));

    stack_eval_.clear();
    stack_eval_.emplace_back(root.get(), 0);
    while (stack_eval_.size() > 0) {
      std::pair<inner_node *, int> p = stack_eval_.back();
      if (p.second == 0) {
        stack_eval_.back().second = 1;
        if (p.first->head->is_inner_node()) {
          stack_eval_.emplace_back(&(p.first->head->get_inner_node()), 0);
        }
        for (auto & c : p.first->children) {
          if (c->is_inner_node()) {
            stack_eval_.emplace_back(&(c->get_inner_node()), 0);
          }
        }
      } else {
        stack_eval_.pop_back();
        bool flag = false;

        auto e2 = evaluate_or_apply(*(p.first->head));
        if (e2) {
          p.first->head = std::move(e2);
          flag = true;
        }
        for (auto & c : p.first->children) {
          e2 = evaluate_or_apply(*c);
          if (e2) {
            c = std::move(e2);
            flag = true;
          }
        }
      }
    }
    e = std::move(root->head);
  }

private:

  std::unique_ptr<node> evaluate_or_apply (node const & e) {
    auto p = evaluate(e);
    if (p) { return std::move(p); }
    p = apply(e);
    if (p) { return std::move(p); }
    return nullptr;
  }

  std::unique_ptr<node> evaluate (node const & e) {
    if (e.is_leaf_node()) {
      return nullptr;
    }

    inner_node const & p = e.get_inner_node();
    if (!p.head->is_string_node()) {
      return nullptr;
    }

    // TODO: separate evaluater
    std::string const & name = p.head->get_string_node().value;

    // TODO: dispatch table
    if (name == "Add") {
      return evaluate_add(p);
    }

    if (name == "Mul") {
      return evaluate_mul(p);
    }

    if (name == "Div") {
      return evaluate_div(p);
    }

    return nullptr;

  }

  std::unique_ptr<node> apply (node const & e) {
    if (e.is_inner_node()) {
      inner_node const & ei = e.get_inner_node();
      if (ei.head->is_string_node()) {
        std::string const & name = ei.head->get_string_node().value;
        auto it = rule_named.find(name);
        if (it != rule_named.end()) {
          for (auto const & r : it->second) {
            if (m.match(*r.first, e)) {
              return build(*(r.second));
            }
          }
        }
      }
    }

    for (auto const & r : rule_other) {
      if (m.match(*r.first, e)) {
        return build(*r.second);
      }
    }
    return nullptr;
  }

  std::vector<std::pair<inner_node *, int>> stack_eval_;
  std::vector<inner_node *> stack_build_;

  // TODO: without replace.
  std::unique_ptr<node> build (node const & e) {
    auto f = e.clone();

    if (f->is_leaf_node()) {
      return std::move(f);
    }

    stack_build_.clear();
    stack_build_.push_back(static_cast<inner_node *>(f.get()));

    while (stack_build_.size() > 0) {
      inner_node * e = stack_build_.back();
      stack_build_.pop_back();

      if (!replace(e->head) && !e->head->is_leaf_node()) {
        stack_build_.push_back(static_cast<inner_node *>(e->head.get()));
      }
      for (auto & c : e->children) {
        if (!replace(c) && !c->is_leaf_node()) {
          stack_build_.push_back(static_cast<inner_node *>(c.get()));
        }
      }
    }

    return std::move(f);
  }

  bool replace (std::unique_ptr<node> & e) {
    if (!e) { return false; }
    if (e->is_string_node()) {
      std::string const & name = static_cast<string_node *>(e.get())->value;
      auto it = m.hold.find(name);
      if (it != m.hold.end()) {
        e = it->second->clone();
        return true;
      }
    }
    return false;
  }

  std::unique_ptr<node> evaluate_add (inner_node const & e) {
    if (e.children.size() == 0) { return nullptr; }
    if (e.children.size() == 1) {
      return e.children[0]->clone();
    }

    if (!e.children[0]->is_int_node() || !e.children[1]->is_int_node()) {
      return nullptr;
    }
    long i = e.children[0]->get_int_node().value;
    long j = e.children[1]->get_int_node().value;

    if (e.children.size() == 2) {
      return make_int_node(i + j);
    }

    return nullptr;
  }

  std::unique_ptr<node> evaluate_mul (inner_node const & e) {
    if (e.children.size() == 0) { return nullptr; }
    if (e.children.size() == 1) {
      return e.children[0]->clone();
    }

    if (!e.children[0]->is_int_node() || !e.children[1]->is_int_node()) {
      return nullptr;
    }

    long i = e.children[0]->get_int_node().value;
    long j = e.children[1]->get_int_node().value;

    if (e.children.size() == 2) {
      return make_int_node(i * j);
    }

    return nullptr;
  }

  std::unique_ptr<node> evaluate_div (inner_node const & e) {
    if (e.children.size() == 0) { return nullptr; }
    if (e.children.size() == 1) {
      return e.children[0]->clone();
    }

    if (!e.children[0]->is_int_node() || !e.children[1]->is_int_node()) {
      return nullptr;
    }

    long i = e.children[0]->get_int_node().value;
    long j = e.children[1]->get_int_node().value;
    if (j == 0) { return nullptr; }

    if (e.children.size() == 2) {
      if (j == 1) {
        return make_int_node(i);
      }
      long r = gcd(i, j);
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

    return nullptr;
  }

};

}
