/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#ifndef M45_ARGPARSE_HH
#define M45_ARGPARSE_HH

#include <string>
#include <utility>

namespace m45::argpaser {
  class parser {
    private:
      const std::string name_;
    public:
      parser() = delete;
      explicit parser(std::string name) : name_(std::move(name)) {}
  };
}

#endif // m45/argparse.hh