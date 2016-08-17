#include <trwlang/matcher/matcher.hpp>
#include <trwlang/parse.hpp>

#include <string>

int main () {

  std::string source;
  std::string line;
  while (std::getline(std::cin, line)) {
    source += line;
  }

  auto first = source.begin();
  auto last = source.end();
  auto pattern = trwlang::parse_node(first, last);
  if (!pattern) { return 0; }

  if (pattern) {
    pattern->print(std::cout); std::cout << std::endl;
  } 

  auto subject = trwlang::parse_node(first, last);
  if (!subject) { return 0; }

  if (subject) {
    subject->print(std::cout); std::cout << std::endl;
  } 

  trwlang::matcher matcher;
  std::cout << (matcher.match(*pattern, *subject) ? "true" : "false") << std::endl;
}
