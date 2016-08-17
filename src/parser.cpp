#include <trwlang/parse.hpp>

#include <string>

int main () {

  std::string source;
  std::string line;
  while (std::getline(std::cin, line)) {
    source += line;
  }

  auto node = trwlang::parse_node(source);

  if (node) {
    node->print(std::cout); std::cout << std::endl;
  } 
}
