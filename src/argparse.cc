/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#include <m45/argparse.hh>

#include <format>

namespace m45::argparse {
  // private:
  const Argument* parser::find_by_name_or_aliases_(
    std::string_view token, const Argument& arg
  ) const {
    if (token == arg.name) return &arg;
    if (token.size() == 1 && arg.short_name && token[0] == *arg.short_name)
      return &arg;
    for (const auto& alias : arg.aliases) if (token == alias) return &arg;
    return nullptr;
  }

  // public:
  // TODO: should throw exception if want to reassign?
  parser& parser::about(std::string about) {
    this->about_ = std::move(about);
    return *this;
  }

  parser& parser::add_flag(
    std::string name,
    ShortName short_name,
    StrList aliases,
    std::string description,
    Action action
  ) {
    args_.push_back(Argument::flag(
      std::move(name), short_name, std::move(aliases),
      std::move(description),
      std::move(action)
    ));
    return *this;
  }

  ParseResult parser::parse_args(int argc, char const *argv[]) {
    if (argc <= 1 ) return ParseResult{
      .success = false,
      .err = ParseResult::ParseError::NO_ARGS,
      .err_msg = "no args provided",
    };

    const Argument* arg_match = nullptr;
  
    for (int i = 1; i < argc; i++) {
      std::string_view token = argv[i];

      for (const auto& arg : args_)
        if ((arg_match = find_by_name_or_aliases_(token, arg))) break;
    
      if (!arg_match) return ParseResult{
        .success = false,
        .err = ParseResult::ParseError::UNKNOWN_ARG,
        .err_msg = std::format("unknown argument: {}", token)
      };
    }

    if (std::holds_alternative<FlagArgument>(arg_match->kind)) {
      Invocation invocation{ .arg_metadata = *arg_match, .values = {}};
      if (arg_match->action) arg_match->action(invocation);
    }

    return {};
  }
}
