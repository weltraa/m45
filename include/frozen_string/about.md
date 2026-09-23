# frozen_string

`m45::frozen_string<T, N>` is an immutable, fixed-size string designed for compile-time text and non-type template parameters. It owns its characters directly, with `constexpr` construction, access, iteration, and comparison, without dynamic allocation for character storage.

Header: [frozen_string.hh](frozen_string.hh).

## Class synopsis

`T` is `char`, `wchar_t`, `char8_t`, `char16_t`, or `char32_t`. `N >= 1` is the array extent, including the final slot; the logical string contains `N - 1` elements. Implicit special members are omitted below.

```cpp
namespace m45 {
  template <detail::CharType T, std::size_t N>
  class frozen_string {
  public:
    const T data[N];

    constexpr frozen_string(const T (&literal)[N]);

    [[nodiscard]] constexpr std::size_t size() const noexcept;
    [[nodiscard]] constexpr std::size_t length() const noexcept;
    [[nodiscard]] constexpr std::size_t extent() const noexcept;
    [[nodiscard]] constexpr bool empty() const noexcept;
    [[nodiscard]] constexpr const T* c_str() const noexcept;

    [[nodiscard]] constexpr const T& operator[](std::size_t pos) const noexcept;
    [[nodiscard]] constexpr const T& at(std::size_t pos) const;
    [[nodiscard]] constexpr const T& front() const noexcept;
    [[nodiscard]] constexpr const T& back() const noexcept;

    [[nodiscard]] constexpr const T* begin() const noexcept;
    [[nodiscard]] constexpr const T* end() const noexcept;
    [[nodiscard]] constexpr std::reverse_iterator<const T*> rbegin() const noexcept;
    [[nodiscard]] constexpr std::reverse_iterator<const T*> rend() const noexcept;

    template <std::size_t OtherN>
    [[nodiscard]] constexpr int compare(
      const frozen_string<T, OtherN>& other) const noexcept;

    template <std::size_t OtherN>
    [[nodiscard]] constexpr bool operator==(
      const frozen_string<T, OtherN>& other) const noexcept;
  };
}
```

## Operations at a glance

Here `n = size()` and `m = other.size()`. All operations use constant auxiliary space.

| Operation | Result or effect | Complexity |
| --- | --- | --- |
| [Array construction](#construction-and-storage) | Copies all `N` elements. | O(N) |
| [Copy/move construction](#construction-and-storage) | Creates an independent value. | O(N) |
| [`data`](#construction-and-storage) | Public `const T[N]` array. | O(1) access |
| [`size()`, `length()`](#size-and-extent) | `N - 1`. | O(1) |
| [`extent()`](#size-and-extent) | `N`. | O(1) |
| [`empty()`](#size-and-extent) | Whether `N == 1`. | O(1) |
| [`c_str()`](#c_str) | Pointer to the stored array. | O(1) |
| [`operator[](pos)`](#element-access) | Unchecked stored-element reference. | O(1) |
| [`at(pos)`](#element-access) | Checked logical-element reference. | O(1) access |
| [`front()`, `back()`](#front-and-back) | Endpoint reference. | O(1) |
| [`begin()`, `end()`](#iterators) | Forward range boundaries. | O(1) |
| [`rbegin()`, `rend()`](#iterators) | Reverse range boundaries. | O(1) |
| [`compare(other)`](#compare) | Exactly `-1`, `0`, or `1`. | O(min(n, m) + 1) |
| [`operator==(other)`](#equality) | Logical string equality. | O(1) for unequal lengths; O(n + 1) otherwise |

## Member reference

The constructor and all explicit member functions are `constexpr`. All observers and comparisons are `const`, `[[nodiscard]]`, and `noexcept`, except that `at()` can throw. Returned pointers, references, and iterators remain valid for the lifetime of their owning object.

### Construction and storage

`frozen_string(const T (&literal)[N])` copies the entire array into `const T data[N]`. Type deduction makes `m45::frozen_string text{"M45"}` a `frozen_string<char, 4>`. The source may be a literal or a named array; the resulting object owns its copy.

The final slot is excluded from the logical string. A literal supplies a null terminator there; other arrays are copied as supplied, without terminator validation. Embedded nulls remain part of the logical string.

The array constructor is not declared `noexcept`. Copy and move construction are nonthrowing; both create separate storage. Assignment is deleted by the immutable representation. Construct from `""` for an empty value; there is no default constructor. Destruction is trivial.

### Size and extent

`size()` and `length()` return `N - 1`; `extent()` returns `N`; `empty()` returns whether `N == 1`. These count code units, including embedded nulls, rather than searching for a terminator or counting Unicode characters.

### `c_str()`

Returns `const T*` pointing to `data[0]`, including for an empty value. Null termination follows the supplied array. For an API that reads until a null character, ensure the array contains one; that API will stop at the first null.

### Element access

- `operator[](pos)` returns `const T&` to `data[pos]`. Requires `pos < extent()`; `pos == size()` accesses the final slot. Access outside the stored array has undefined behavior.
- `at(pos)` returns `const T&` when `pos < size()`; otherwise it throws `std::out_of_range`. It rejects the final slot and every position in an empty string.

### `front()` and `back()`

`front()` returns `data[0]`; `back()` returns `data[size() - 1]`. For a nonempty string, these are its first and last logical characters.

On an empty string, `front()` accesses the sole stored slot (`'\0'` for `""`). `back()` requires `!empty()`; calling it on an empty string has undefined behavior.

### Iterators

`begin()` and `end()` return `const T*` delimiting `[data, data + size())`. `rbegin()` and `rend()` are `std::reverse_iterator<const T*>` constructed from `end()` and `begin()`, respectively.

Both ranges traverse the logical characters, including embedded nulls and excluding the final slot. Both are empty when `empty()` is true. Forward iterators provide contiguous, random-access traversal; all iterators provide read-only access. Reverse end iterators must not be dereferenced.

### `compare()`

Compares logical strings lexicographically, returning `-1`, `0`, or `1` for less, equal, or greater. The first unequal code unit determines the result; if one string is a prefix, the shorter comes first. Ordering uses built-in `T` comparisons, so plain `char` signedness can affect non-ASCII ordering.

The argument has the same `T` and may have a different extent. For ordinary type deduction, pass another `frozen_string`, such as `text.compare(m45::frozen_string{"M45"})`. An explicit extent also permits array conversion: `text.compare<4>("M45")`.

### Equality

`operator==` returns whether the logical lengths and all logical characters match. As with `compare()`, the character types must match and the final stored slot is ignored. Different lengths compare unequal immediately; embedded nulls participate normally. The language supplies `!=` through equality rewriting.

## Current limitations

Search, prefix/suffix checks, substrings, ordering operators (including `<=>`), and implicit conversion to `std::basic_string_view` are not currently provided.

For standard string-view operations, construct `std::basic_string_view<T>{text.c_str(), text.size()}` explicitly; the view preserves embedded nulls and refers to `text`'s storage.

## License

Licensed under the [Apache License, Version 2.0](LICENSE).
