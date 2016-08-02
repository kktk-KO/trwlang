#pragma once

#include <trwlang/node.hpp>

#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace trwlang {

struct expr {

  expr (std::unique_ptr<node> root)
  : root_(std::move(root)) {
  }

private:
  std::unique_ptr<node> root_;
};

}
