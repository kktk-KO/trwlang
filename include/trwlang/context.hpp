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
      auto & head = *(p2.children[0]);
      if (head.is_string_node()) {
        std::string const & name = head.get_string_node().value;
        rule_named[name].emplace_back(std::move(p), std::move(e));
        return;
      }
    } else if (p->is_string_node()) {
      std::string const & name = p->get_string_node().value;
      rule_named[name].emplace_back(std::move(p), std::move(e));
      return;
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
        for (auto & c : p.first->children) {
          if (c->is_inner_node()) {
            stack_eval_.emplace_back(&(c->get_inner_node()), 0);
          }
        }
      } else {
        stack_eval_.pop_back();
        bool flag = false;

        for (auto & c : p.first->children) {
          auto e2 = evaluate_or_apply(*c);
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
    e = std::move(root->children[0]);
  }

  void evaluate_once (std::unique_ptr<node> & e) {
    auto root = make_inner_node(std::move(e));

    stack_eval_.clear();
    stack_eval_.emplace_back(root.get(), 0);
    while (stack_eval_.size() > 0) {
      std::pair<inner_node *, int> p = stack_eval_.back();
      if (p.second == 0) {
        stack_eval_.back().second = 1;
        for (auto & c : p.first->children) {
          if (c->is_inner_node()) {
            stack_eval_.emplace_back(&(c->get_inner_node()), 0);
          }
        }
      } else {
        stack_eval_.pop_back();
        bool flag = false;

        for (auto & c : p.first->children) {
          auto e2 = evaluate_or_apply(*c);
          if (e2) {
            c = std::move(e2);
            flag = true;
          }
        }
      }
    }
    e = std::move(root->children[0]);
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

  std::unique_ptr<node> apply (node const & e) {
    if (e.is_inner_node()) {
      inner_node const & ei = e.get_inner_node();
      if (ei.children[0]->is_string_node()) {
        std::string const & name = ei.children[0]->get_string_node().value;
        auto it = rule_named.find(name);
        if (it != rule_named.end()) {
          for (auto const & r : it->second) {
            if (m.match(*r.first, e)) {
              return build(*(r.second));
            }
          }
        }
      }
    } else if (e.is_string_node()) {
      std::string const & name = e.get_string_node().value;
      auto it = rule_named.find(name);

      if (it != rule_named.end()) {
        for (auto const & r : it->second) {
          if (m.match(*r.first, e)) {
            return build(*(r.second));
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
    stack_build_.push_back(&(f->get_inner_node()));

    while (stack_build_.size() > 0) {
      inner_node * e = stack_build_.back();
      stack_build_.pop_back();

      for (auto & c : e->children) {
        if (!replace(c) && c->is_inner_node()) {
          stack_build_.push_back(&(c->get_inner_node()));
        }
      }
    }

    return std::move(f);
  }

  bool replace (std::unique_ptr<node> & e) {
    if (!e) {
      return false;
    }

    if (!e->is_string_node()) {
      return false;
    }

    auto it = m.hold.find(e->get_string_node().value);
    if (it == m.hold.end()) {
      return false;
    }
    e = it->second->clone();
    return true;
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
