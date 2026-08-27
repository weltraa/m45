/**
 * M45 argparse
 * Copyright (c) 2026 weltraa
 * Licensed under the [LICENSE NAME] License.
 */

#include <m45/argparse.hh>

#include <format>
#include <cctype>
#include <stdexcept>
#include <charconv>

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

    std::optional<ParsedValue> parse_value_of_key(
      const std::string& value_of_key,
      ValuedArgument::ValuedArgType value_type
    ) {
      switch (value_type){
        case ValuedArgument::ValuedArgType::String: return value_of_key;
        case ValuedArgument::ValuedArgType::Integer: {
          int num = 0;
          auto result = std::from_chars(
            value_of_key.data(),
            value_of_key.data() + value_of_key.size(),
            num
          );

          return result.ec == std::errc{}
              ? std::optional<int>{num}
              : std::nullopt;
        }
        case ValuedArgument::ValuedArgType::Boolean:
          if (value_of_key == "true") return std::optional<bool>{true};
          else if (value_of_key == "false") return std::optional<bool>{false};
          else return std::nullopt;
      }

      return std::nullopt;
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

  /* split_key_and_value:
   * Returns the name whenever it matches, even when the token carries no '='.
   * That is what lets parse_args() tell a known arg used wrong apart from an
   * unknown one, instead of both falling through to UNKNOWN ARG.
   * The '=' is still mandatory:
   * a missing separator is now reported as MISSING VALUE.
  */
  std::pair<std::string, std::optional<std::string>>
  parser::split_key_and_value_(
    std::string_view token, const Argument& arg
  ) const {
    const auto separator = token.find('=');
    const auto key = separator == std::string_view::npos
      ? token
      : token.substr(0, separator);

    if (key != arg.name) return {"", std::nullopt};
    if (separator == std::string_view::npos)
      return {std::string{key}, std::nullopt};

    return {std::string{key}, std::string{token.substr(separator + 1)}};
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

  parser& parser::add_valued(
    std::string name,
    ValuedArgument::ValuedArgType value_type,
    Arity arity,
    Action action
  ) {
    if (!is_dashed_name(name))
      throw std::invalid_argument(
        std::format("invalid valued arg name '{}'", name)
      );

    args_.push_back(Argument::valued(
      std::move(name), value_type, std::move(arity), std::move(action)
    ));

    return *this;
  }

  ParseResult parser::parse_args(int argc, char const *argv[]) {
    if (argc <= 1 ) return ParseResult{
      .success = false,
      .err = ParseResult::ParseError::NO_ARGS,
      .err_msg = "no args provided",
    };

    std::vector<std::vector<ParsedValue>> collected(args_.size());
    std::vector<std::uint32_t> occurrences(args_.size(), 0);

    for (int i = 1; i < argc; i++) {
      std::string_view token = argv[i];

      const Argument* arg_match = nullptr;
      std::size_t match_index = 0;
      std::optional<std::string> value_of_key;

      // TODO: maybe use switch instead of if's
      // Indexed for instead of forrange so the match can be tied back
      // to its slot in 'collected' and 'occurrences'.
      for (std::size_t idx = 0; idx < args_.size(); ++idx) {
        const auto& arg = args_[idx];
        if(
          std::holds_alternative<FlagArgument>(arg.kind) ||
          std::holds_alternative<CommandArgument>(arg.kind)
        ){
          if ((arg_match = find_by_name_(token, arg))) {
            match_index = idx;
            break;
          }
        }
        else {
          auto [key, value] = split_key_and_value_(token, arg);
          if(key.empty()) continue;

          arg_match = &arg;
          match_index = idx;
          value_of_key = std::move(value);
          break;
        }
      }

      if (!arg_match) return ParseResult{
        .success = false,
        .err = ParseResult::ParseError::UNKNOWN_ARG,
        .err_msg = std::format("unknown argument: {}", token)
      };

      const auto& matched_arity = arg_match->arity;
      if (
        matched_arity.max &&
        occurrences[match_index] + 1 > *matched_arity.max
      ) return ParseResult{
        .success = false,
        .err = ParseResult::ParseError::TOO_MANY_OCCURRENCES,
        .err_msg = std::format(
          "{} may appear at most {} time(s)",
          arg_match->name, *matched_arity.max
        )
      };
      occurrences[match_index]++;

      switch (arg_match->kind.index()) {
        // FlagArgument
        case 0: {
          Invocation invocation{ .arg_metadata = *arg_match, .values = {}};
          if (arg_match->action) arg_match->action(invocation);
          break;
        }
        // ValuedArgument
        case 1: {
          auto arg_name = arg_match->name;
          if (!value_of_key)
            return ParseResult{
              .success = false,
              .err = ParseResult::ParseError::MISSING_VALUE,
              .err_msg = std::format(
                "{} requires a value ({}=VALUE)", arg_name, arg_name
              )
            };
          if (value_of_key->empty())
            return ParseResult{
              .success = false,
              .err = ParseResult::ParseError::EMPTY_VALUE,
              .err_msg = std::format(
                "{} requires a non-empty value ({}=VALUE)", arg_name, arg_name
              )
            };

          auto value_kind =
            std::get<ValuedArgument>(arg_match->kind).value_kind;
          auto parsed_value = parse_value_of_key(*value_of_key, value_kind);

          if (!parsed_value.has_value())
            return ParseResult{
              .success = false,
              .err = ParseResult::ParseError::INVALID_VALUE,
              .err_msg = std::format(
                "TODO: implement this err msg later"
              )
            };

          auto& slot = collected[match_index];
          slot.push_back(std::move(*parsed_value));

          Invocation invocation{
            .arg_metadata = *arg_match,
            .values = std::span<const ParsedValue>{&slot.back(), 1}
          };
          if (arg_match->action) arg_match->action(invocation);
          break;
        }
      default:
        break;
      }
    }

    for (std::size_t idx = 0; idx < args_.size(); ++idx) {
      const auto& arg = args_[idx];
      if (occurrences[idx] >= arg.arity.min) continue;

      return ParseResult{
        .success = false,
        .err = ParseResult::ParseError::MISSING_OCCURRENCES,
        .err_msg = std::format(
          "{} must appear at least {} time(s), got {}",
          arg.name, arg.arity.min, occurrences[idx]
        )
      };
    }

    return {};
  }
}
