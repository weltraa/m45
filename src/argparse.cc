/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#include <m45/argparse.hh>

#include <format>
#include <cctype>
#include <stdexcept>

namespace m45::argparse {
  // utilities:
  namespace {
    bool is_dashed_name(std::string_view str) noexcept {
      if (!str.starts_with("--")) return false;

      const auto suffix = str.substr(2);
      if (suffix.empty()) return false;
      if (suffix.back() == '-') return false;

      for (char ch : suffix)
        if (!std::isalpha(static_cast<unsigned char>(ch)) && ch != '-') 
          return false;

      return true;
    }

    bool is_valid_short_name(std::string_view short_name) {
      return
          short_name.size() == 2 &&
          short_name[0] == '-' &&
          std::isalpha(static_cast<unsigned char>(short_name[1]));
    }
  }

  // private:
  const Argument* parser::find_by_name_(
    std::string_view token, const Argument& arg
  ) const {
    if (token == arg.name) return &arg;
    if (arg.short_name && token == *arg.short_name) return &arg;
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
    std::string name, std::optional<std::string> short_name,
    std::string description,
    bool negatable,
    Arity arity,
    Action action
  ) {
    if (!is_dashed_name(name))
      throw std::invalid_argument(std::format("invalid flag name '{}'", name));
    if(short_name && !is_valid_short_name(*short_name))
      throw std::invalid_argument(
        std::format(
          "invalid flag short name '{}': must be alphabetic",
          *short_name
        )
      );

    args_.push_back(Argument::flag(
      std::move(name),
      std::move(short_name),
      std::move(description),
      negatable,
      std::move(arity),
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
        if ((arg_match = find_by_name_(token, arg))) break;
    
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
