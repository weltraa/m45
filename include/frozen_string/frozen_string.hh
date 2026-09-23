//
// Copyright (c) 2026 weltraa
//
// This header file is an independent member of the M45 library.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef M45_FROZEN_STRING_FROZEN_STRING_HH
#define M45_FROZEN_STRING_FROZEN_STRING_HH

#include <cstddef>
#include <concepts>
#include <utility>
#include <stdexcept>
#include <iterator>

namespace m45 {

  namespace detail {

    template <typename T>
    concept CharType =
      std::same_as<T, char>     ||
      std::same_as<T, wchar_t>  ||
      std::same_as<T, char8_t>  ||
      std::same_as<T, char16_t> ||
      std::same_as<T, char32_t>;  
  }

  // Pending methods:
  // Candidates:
  // - operator <=>
  // - starts_with()
  // - ends_with()
  // - contains()
  // - find()
  // - rfind()
  // - find_first_of()
  // - find_last_of()
  // - find_first_not_of()
  // - find_last_not_of()
  // - operator std::basic_string_view<T>()
  // - substr()
  template <detail::CharType T, std::size_t N>
  class frozen_string {
    public:
      // attributes {
      const T data[N];
      //}

      // methods {
      constexpr frozen_string(const T(&literal)[N])
        : frozen_string(literal, std::make_index_sequence<N>{}) {}

      [[nodiscard]] constexpr 
      std::size_t size(void)
      const noexcept
      { return N - 1; }

      [[nodiscard]] constexpr
      std::size_t length(void)
      const noexcept
      { return N - 1; }

      [[nodiscard]] constexpr
      std::size_t extent(void)
      const noexcept
      { return N; }

      [[nodiscard]] constexpr
      bool empty(void)
      const noexcept
      { return size() == 0; }

      [[nodiscard]] constexpr
      const T* c_str(void)
      const noexcept
      { return data; }

      [[nodiscard]] constexpr
      const T& operator[](std::size_t pos)
      const noexcept
      { return data[pos]; }

      [[nodiscard]] constexpr
      const T& at(std::size_t pos) const {
        return (pos < size())
          ? data[pos]
          : throw std::out_of_range(
              "frozen_string::at: position out of range"
            );
      }

      [[nodiscard]] constexpr
      const T& front(void)
      const noexcept
      { return data[0]; }

      [[nodiscard]] constexpr
      const T& back(void)
      const noexcept
      { return data[size() - 1]; }

      [[nodiscard]] constexpr
      const T* begin(void)
      const noexcept
      { return data; }

      [[nodiscard]] constexpr
      const T* end(void)
      const noexcept
      { return data + size(); }

      [[nodiscard]] constexpr
      std::reverse_iterator<const T*> rbegin(void)
      const noexcept
      { return std::reverse_iterator<const T*>(data + size()); }

      [[nodiscard]] constexpr
      std::reverse_iterator<const T*> rend(void)
      const noexcept
      { return std::reverse_iterator<const T*>(data); }
      
      template<std::size_t OtherN>
      [[nodiscard]] constexpr
      int compare(const frozen_string<T, OtherN>& other)
      const noexcept {
        const auto min_size =
          size() < other.size()
            ? size()
            : other.size();

        for (std::size_t i = 0; i < min_size; ++i) {
          if (data[i] < other.data[i]) return -1;
          if (data[i] > other.data[i]) return 1;
        }

        if (size() < other.size()) return -1;
        if (size() > other.size()) return 1;

        return 0;
      }

      template<std::size_t OtherN>
      [[nodiscard]] constexpr
      bool operator==(const frozen_string<T, OtherN>& other)
      const noexcept {
        if (size() != other.size()) return false;

        for (std::size_t i = 0; i < other.size(); ++i)
          if (data[i] != other.data[i]) return false;

        return true;
      }
      //}

    private:
      // methods {
      template<std::size_t... I>
      constexpr frozen_string(
        const T (&literal)[N],
        std::index_sequence<I...>
      ) : data{literal[I]...} {}
      //}
  };
}

#endif // M45_FROZEN_STRING_FROZEN_STRING_HH