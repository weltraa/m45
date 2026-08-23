/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#include <m45/argparse.hh>
#include <iostream>
using namespace m45;


int main(int argc, char const *argv[]) {

  argpaser::parser parser{"hello"};
  std::cout << parser.name << '\n';
  
  return 0;
}

