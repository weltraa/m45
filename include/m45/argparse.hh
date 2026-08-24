/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#ifndef M45_ARGPARSE_HH
#define M45_ARGPARSE_HH

#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <cstdint>
#include <variant>
#include <span>
#include <string_view>
#include <functional>

namespace m45::argparse {
  using ShortName = std::optional<char>;
  using StrList = std::vector<std::string>;

  struct Arity {
    std::uint16_t min = 1;
    std::optional<std::uint16_t> max = 1;
  };

  struct Argument;
  struct Invocation {
    const Argument& arg_metadata;
    std::span<const std::string_view> values;
  };
  using Action = std::function<void(const Invocation&)>;

  struct FlagArgument {
    bool negatable = false;
  };
  struct ValuedArgument {
    enum class ValueKind {
      String, Integer, Boolean
    };

    ValueKind value_kind = ValueKind::String;
  };
  struct CommandArgument {};
  using ArgumentKind = std::variant<
    FlagArgument,
    ValuedArgument,
    CommandArgument
  >;

  struct Argument {
    // attrs {
      std::string name;
      ShortName short_name;
      StrList aliases;
      std::string description;

      ArgumentKind kind = FlagArgument{};
      Arity arity;

      StrList choices;

      Action action;
    //}

    // factory methods {
      static Argument flag(
        std::string name, ShortName short_name, StrList aliases,
        std::string description,
        Action action
      ) {
        Argument arg;
        arg.name = std::move(name);
        arg.short_name = short_name;
        arg.aliases = std::move(aliases);
        arg.description = std::move(description);
        arg.action = std::move(action);

        return arg;
      }
    //}
  };

  struct ParseResult {
    enum class ParseError {
      NONE,
      NO_ARGS,
      UNKNOWN_ARG,
    };

    bool success = true;
    ParseError err = ParseError::NONE;
    std::string err_msg;

    explicit operator bool() const noexcept {
      return success;
    }
  };

  class parser {
    private:
      const std::string name_;
      std::string about_;
      std::vector<Argument> args_;
      
      const Argument* find_by_name_or_aliases_(
        std::string_view token, const Argument& arg
      ) const;
    public:
      parser() = delete;
      explicit parser(std::string name) : name_(std::move(name)) {}

      parser& about(std::string about);
      parser& add_flag(
        std::string name, ShortName short_name, StrList aliases,
        std::string description,
        Action action
      );
      ParseResult parse_args(int argc, char const *argv[]);
  };
}

#endif // m45/argparse.hh