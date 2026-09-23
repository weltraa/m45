<div align='center'>
  <img src='assets/imgs/M45.png' alt='M45 Logo' width='50%' />
</div>

## About

**M45** is a C++26-and-beyond library that provides components and abstractions not yet available in the **C++ Standard Library**.

It serves as a foundational dependency for many of my other projects, filling gaps in the standard library with reusable, general-purpose components.

M45 is entirely **header-only**. Each header file is provided as an independent `.hh` header and can be included individually, allowing projects to depend only on the facilities they actually use. An umbrella header is planned for a future release, once the library includes more modules, to provide a single include for the complete M45 library.

M45 follows the evolution of the C++ standard. Each component is maintained for the language versions in which its abstraction is still missing. Once an equivalent facility becomes part of the C++ Standard Library, the corresponding M45 component is considered superseded, and users are encouraged to migrate to the standard alternative.

For example, suppose `m45::frozen_string` provides an abstraction that is unavailable in C++26 and remains unavailable in C++29. M45 would continue supporting the component for those language versions. If a future standard (let's say C++32) introduced an equivalent standard facility, `m45::frozen_string` could then be marked as superseded/deprecated, with users encouraged to adopt the standard alternative instead.

## Changelog

- **M45 v1.0.0-beta.1** — Initial beta release of the header-only library, introducing independently versioned components.
- **`frozen_string` v1.0.0-beta.1** — Initial beta release with fixed-size, immutable strings, `constexpr` construction, support for standard character types, element access, forward and reverse iteration, lexicographical comparison, and equality checks.

## Documentation

- [`frozen_string`](include/frozen_string/about.md) — Fixed-size, immutable strings with `constexpr` support.

## License

Licensed under the [Apache License, Version 2.0](LICENSE).
