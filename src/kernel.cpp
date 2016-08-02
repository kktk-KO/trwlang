
#include <iostream>

#include <trwlang/expr.hpp>
#include <trwlang/node.hpp>
#include <trwlang/inner_node.hpp>
#include <trwlang/leaf_node.hpp>
#include <trwlang/string_node.hpp>
#include <trwlang/int_node.hpp>
#include <trwlang/match.hpp>
#include <trwlang/parse.hpp>
#include <trwlang/context.hpp>

#include <vector>

int main () {
  using namespace trwlang;

  std::string line;

  context ctx;

  while (true) {
    std::cout << "In  = ";
    if (!std::getline(std::cin, line)) {
      return 0;
    }
    auto first = line.begin();
    auto last = line.end();
    auto e = parse_expr(first, last);
    if (!e) {
      std::cout << "parse error!" << std::endl;
      continue;
    }

    if (e->is_inner_node() && check_head(e->get_inner_node(), "Rule")) {
      auto & f = e->get_inner_node();
      ctx.add_rule(std::move(f.children[0]), std::move(f.children[1]));
      std::cout << "add new rule" << std::endl;
      continue;
    }

    ctx.evaluate_all(e);
    std::cout << "Out = ";
    e->print(std::cout); std::cout << std::endl;    
  }
}
