/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#include <m45/argparse.hh>
#include <iostream>
#include <variant>
using namespace m45;

void printVersion(const m45::argparse::Invocation&) {
  std::cout << "hello -   1.0.0\n";
}

void printStd(const m45::argparse::Invocation& invocation) {
  for (const auto& value : invocation.values)
    std::visit(
      [](const auto& held) { std::cout << "std: " << held << '\n'; },
      value
    );
}

int main(int argc, char const *argv[]) {
  argparse::parser parser{"hello"};
  parser
    .about("a compiler.")
    .add_flag(
      "--version", std::nullopt,
      "Print version information",
      false, {0, 1}, printVersion
    )
    .add_valued(
      "--std", argparse::ValuedArgument::ValuedArgType::String,
      {1, 1}, printStd
    );

  auto result = parser.parse_args(argc, argv);

  if (!result) {
    std::cerr << result.err_msg << '\n';
    return 1;
  }
  
  return 0;
}

