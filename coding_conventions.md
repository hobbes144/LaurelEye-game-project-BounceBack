# LaurelEye Coding Conventions

## Header File Includes

- **Include What You Use**: Never rely on someone else’s header to
include what you need. All used types (that need definition) must have their
headers directly included.
- **Don't Include What You Don't Need To Know**: Prefer forward declarations
to keep headers light, only include when you need completeness.
    - Cases where we forward declare:
        - References
        - Pointers
- **Include order**: Categories are separated by a new line and includes are
sorted alphabetically within each category.
    - Base Class (`.h` files) / Header (`.cpp` files)
    - Project headers
    - 3rd party libraries
    - C/C++ Standard library headers
- **Avoid using namespace in headers (unless namespace block)**: It pollutes all
includers. Use fully qualified names in headers or use namespace blocks.
- **Guards**: Use `#pragma once` only. Don't add both pragma and include guards.

# GAM55X Coding Conventions

Your team is free to update this coding convention as you see fit. It is provided as a guideline.  However, when revising please ensure that the team adheres to the general principles outlined below.

## General Principles

- Keep it simple.
- Consistency is king: Pick a style and stick with it across the codebase.
- Descriptive over clever: Names should convey intent, not obscure it.
- Avoid abbreviations unless they’re universally understood (e.g., `idx`, `str`, `buf`).
- Try adding a `.clang-format` with a configuration file aligned to these conventions. It's not a perfect tool and often teams adapt their coding style to whatever the tools can support.

## Why Naming Conventions Matter

- **Readability**: Consistent names make code easier to understand and maintain.
- **Collaboration**: Uniform conventions help teams work together without confusion.
- **Professionalism**: Following industry-aligned standards (e.g., ISO C++, Java) prepares you for real-world projects.
- **Debugging**: Descriptive names reduce errors and simplify debugging.

## File Naming

- **Style:** `snake_case` or `PascalCase` as appropriate
- **Examples:** `logger.h`, `string_utils.cpp`, `strings.cppm`
- **Notes:** File names should match the primary namespace or class they define. Header files use `.h`, source files use `.cpp`, and module interfaces use `.cppm` (C++23).

## Documentation

- Use Doxygen-style comments for classes, methods, and functions.
- Document intent and parameters clearly, e.g.:

  ```cpp
  /// @brief Transcodes a string from UTF-8 to UTF-16.
  /// @param input UTF-8 encoded string.
  /// @return UTF-16 encoded string.
  std::u16string transcode(std::string_view input);
  ```

- Keep comments concise and avoid duplicating obvious code details.

## Names

Typically, a team's naming conventions borrow heavily from an existing convention [Oracle's Java Naming Conventions](https://www.oracle.com/java/technologies/javase/codeconventions-namingconventions.html) for object-oriented programming (OOP) name styles, largely because they are familiar to most students and not particularly objectionable. Most C++ code mixes OOP and functional programming (FP), so it's also appropriate to borrow FP standards, such as those in the ISO standard naming seen in the C++ standard library.

These suggestions below were designed for C++23, aligning with modern C++ practices like concepts, ranges, and modules.

### Summary

| Category                | Style                    | Examples                           | Notes                                                                 |
|-------------------------|--------------------------|------------------------------------|----------------------------------------------------------------------|
| Namespaces              | `UpperCamelCase`             | `Aspen`, `Strings`                | Prefer single-word names.                                            |
| Modules                 | `snake_case`             | `aspen.strings`, `aspen.utils`   | Nouns; match namespace names.                                        |
| Classes                 | `UpperCamelCase`         | `Logger`, `TransformString`        | Nouns; verbs for function objects.                                   |
| Methods                 | `lowerCamelCase`         | `getValue`, `parseInput`           | Verbs describing actions.                                            |
| Functions               | `lower_snake_case`       | `transcode`, `trim_leading_whitespace` | Verbs; matches C-style functions.                               |
| Variables/Parameters    | `lowerCamelCase`         | `inputBuffer`, `userId`, `str`     | Nouns; use well-known abbreviations (e.g., `id`, `str`).             |
| Fields (Public)         | `lowerCamelCase`         | `value`, `parsedText`              | Nouns; used in simple data structs.                                  |
| Fields (Private/Protected) | `lowerCamelCase_`     | `channels_`, `fileMutex_`          | Nouns; accessed via methods.                                         |
| Enum Classes            | Class: `UpperCamelCase`<br>Values: `lower_snake_case` | `ErrorCode::invalid_argument` | Nouns; `snake_case` for values to distinguish from fields.           |
| Template Parameters     | `UpperCamelCase`         | `T`, `Char`, `CharTraits`          | Nouns; short names, no `T` prefix.                                   |
| Concepts                | `UpperCamelCase`         | `Sortable`, `InputIterator`        | Adjectives/Nouns; describe type requirements.                        |
| Constants/Macros        | `ALL_CAPS_SNAKE_CASE`    | `MAX_CHAR_COUNT`, `ALWAYS_INLINE`  | Nouns; verbs for function-like macros.                               |
| Niebloids               | Type: `lower_snake_case_t`<br>Symbol: `lower_snake_case` | `to_upper_t`, `to_upper` | Verbs; follows ISO C++ for customization point objects.              |

### Details

##### Namespaces

- **Style:** `UpperCamelCase`
- **Examples:** `Aspen`, `Strings`
- **Part of Speech:** Nouns and Noun Phrases
- **Notes:** Prefer single-word names over `MultipleWords`.

##### Modules (*future*)

- **NOTE:** C++ Modules are new and most students don't use them yet. Moreover, there's limited support in the MS VC toolset. ***Recommended*:** If you don't already know how to use modules, don't try to use them in GAM55X.
- **Style:** `snake_case`
- **Examples:** `aspen.strings`, `aspen.utils`
- **Part of Speech:** Nouns or Noun Phrases
- **Notes:** Module names should match namespace names for consistency. Use dot notation for submodules. For example:

  ```cpp
  export module aspen.strings;
  export namespace aspen::strings { /* ... */ }
  ```

##### Classes

- **Style:** `UpperCamelCase`
- **Examples:** `Logger`, `TransformString`
- **Part of Speech:** Nouns and Noun Phrases, except function objects, which use Verbs or Verb Phrases.
- **Notes:** Classes should represent clear, cohesive abstractions. Function objects (functors) use verb-based names, e.g.:

  ```cpp
  struct TransformString {
      std::string operator()(std::string_view input) const { /* ... */ }
  };
  ```

##### Methods

- **Style:** `lowerCamelCase`
- **Examples:** `getValue`, `parseInput`
- **Part of Speech:** Verbs or Verb Phrases
- **Notes:** Methods should describe actions or behaviors of the class.

##### Functions (Namespace Scope)

- **Style:** `lower_snake_case`
- **Examples:** `transcode`, `trim_leading_whitespace`
- **Part of Speech:** Verbs or Verb Phrases
- **Notes:** Namespace-scoped functions act like C-style functions, matching naming in most C++ libraries. Inline functions use the same style, e.g., `inline void log_error(...)`.

##### Variables and Parameters

- **Style:** `lowerCamelCase`
- **Examples:** `inputBuffer`, `userId`, `str`
- **Part of Speech:** Nouns and Noun Phrases
- **Notes:** Use descriptive names, employing only well-known abbreviations (e.g., `id`, `str`). Inline variables follow the same style, e.g., `inline int defaultBufferSize = 1024;`.
- **Never:** Do NOT use *Hungarian style names* e.g. `szBuffer`

##### Public Member Fields

- **Style:** `lowerCamelCase`
- **Examples:** `value`, `parsedText`
- **Part of Speech:** Nouns and Noun Phrases
- **Notes:** Public fields are generally used only in simple data structs, i.e., composite data types without complex behavior. For example:

  ```cpp
  struct FormatOptions {
      int base;
      bool useScientificNotation;
  };
  ```

##### Private Member Fields

- **Style:** `lowerCamelCase_`
- **Examples:** `channels_`, `fileMutex_`
- **Part of Speech:** Nouns and Noun Phrases
- **Never:** `m_fieldName` or `_fieldName` of `_fieldName_`
- **Notes:** Even simple readable/writable fields should be private and accessed through public methods. For example:

  ```cpp
  class Channel {
  public:
      Level level() const { return level_; }
      void level(Level newValue) { level_ = newValue; }
  private:
      Level level_;
  };
  ```

##### Enum Classes

- **Style:** Class names: `UpperCamelCase`, Value names: `lower_snake_case`
- **Examples:**

  ```cpp
  enum class ErrorCode {
      invalid_argument,
      out_of_range
  };
  ```

- **Part of Speech:** Nouns and Noun Phrases for both
- **Notes:** Prefer descriptive names. Enum value names use `snake_case` to distinguish them from public field names.

##### Template Parameters

- **Style:** `UpperCamelCase`
- **Examples:** `T`, `Char`, `CharTraits`
- **Part of Speech:** Nouns and Noun Phrases
- **Notes:** Prefer short names. Do not prefix with `T` as in `TCharTraits`.

##### Concepts

- **Style:** `UpperCamelCase`
- **Examples:** `Sortable`, `InputIterator`
- **Part of Speech:** Adjectives or Nouns describing properties
- **Notes:** Concepts should describe type requirements clearly. Avoid generic names like `Concept1`. For example:

  ```cpp
  template<typename T>
  concept Sortable = requires(T t) { /* sorting requirements */ };
  ```

##### Constants and Macros

- **Style:** `ALL_CAPS_SNAKE_CASE`
- **Examples:** `MAX_CHAR_COUNT`, `ALWAYS_INLINE`
- **Part of Speech:** Nouns and Noun Phrases for values; function-like macros may use Verbs or Verb Phrases if appropriate.
- **Notes:** Prefer descriptive names.

##### Niebloids

- **NOTE:** Niebloids are common in utility libraries, but uncommon in game engines and video games. See Neibloids.md for additional information.
- **Style:** Niebloid type name: `lower_snake_case_t`, Symbol: `lower_snake_case`
- **Examples:**

  ```cpp
  namespace aspen {
      struct to_upper_t {
          std::string operator()(std::string_view s) const { /* ... */ }
      };
      constexpr to_upper_t to_upper{};
  }
  ```

- **Part of Speech:** Verbs or Verb Phrases
- **Notes:** Niebloids are customization point objects, like those in C++20’s ranges library, allowing flexible, non-intrusive function customization. Use for operations like transformations or predicates.

## Common Pitfalls

- **Mixing Styles**: Don’t use `snake_case` for class names or `CamelCase` for functions.
- **Overusing Abbreviations**: Avoid cryptic names like `bufSz` instead of `bufferSize`.
- **Ignoring Encapsulation**: Always make fields private unless they’re simple data struct members.
- **Misusing Niebloids**: Ensure niebloids are `constexpr` and follow ISO naming for consistency.

## C++ Brace Placement

We use the K&R (Kernighan and Ritchie) brace style for C++ code to prioritize vertical compactness while maintaining clarity. Opening braces are placed on the same line as the declaration or statement, and closing braces are placed on a new line, aligned with the start of the statement. This approach reduces vertical space usage compared to styles like Allman, making the code more concise without sacrificing readability.

#### Guidelines

- **Function Declarations/Definitions**: Place the opening brace on the same line as the function signature.
- **Control Structures**: Place the opening brace on the same line as the control statement (e.g., `if`, `for`, `while`).
- **Classes/Structs**: Place the opening brace on the same line as the class or struct declaration.
- **Namespaces**: Follow the same rule, with the opening brace on the same line as the namespace declaration.
- **Single-line statements**: Braces are optional for single-line statements but recommended for consistency if the block may expand later.

#### Example

```cpp
class MyClass {
public:
    void myMethod(int x) {
        if (x > 0) {
            std::cout << "Positive" << std::endl;
        } else {
            std::cout << "Non-positive" << std::endl;
        }
    }
};

namespace MyNamespace {
    int compute() {
        for (int i = 0; i < 10; i++) {
            std::cout << i << std::endl;
        }
        return 0;
    }
}
```

#### Rationale

The K&R style minimizes vertical space, allowing more code to be visible on the screen, which is especially beneficial for large codebases or when working with limited screen real estate. It maintains a clean, consistent look while clearly delineating scope boundaries.
