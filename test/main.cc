/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#include <m45/argparse.hh>
#include <iostream>
using namespace m45;

void printVersion(const m45::argparse::Invocation&) {
  std::cout << "hello -   1.0.0\n";
}

int main(int argc, char const *argv[]) {

  argparse::parser parser{"hello"};
  parser
    .about("a compiler.")
    .add_flag(
      "version", 'v', {},
      "Print version information",
      printVersion
    );

  auto result = parser.parse_args(argc, argv);

  if (!result) {
    std::cerr << result.err_msg << '\n';
    return 1;
  }
  
  return 0;
}

