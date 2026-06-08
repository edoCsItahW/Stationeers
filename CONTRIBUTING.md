## Contributing to IC10 Compiler

Thank you for your interest in contributing! This document outlines the process and guidelines for contributing to the IC10 Compiler project.

### Code of Conduct

By participating, you agree to uphold a respectful and inclusive environment. Please report unacceptable behavior to the project maintainer.

### How to Contribute

1. **Report bugs** – Use GitHub Issues, describe the problem, include reproduction steps and environment details.
2. **Suggest enhancements** – Open an issue labeled `enhancement` with a clear description of the proposed feature.
3. **Submit pull requests** – Follow the process below.

### Development Workflow

1. **Fork** the repository.
2. **Create a feature branch** from `develop` (or `main` for urgent fixes).
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make changes** – Keep commits logical, write clear messages.
4. **Run tests** locally:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   cd build && ctest --output-on-failure
   ```
5. **Ensure code style** – The project uses `clang-format`. Run:
   ```bash
   clang-format -i <file>
   ```
6. **Push** and open a Pull Request to `develop`.

### Coding Standards

- **Language**: C++23 (use coroutines, concepts, ranges where appropriate).
- **Style**: Follow the existing pattern (2 spaces indentation, braces on same line). See `.clang-format` (if present).
- **Naming**:
    - Types: `PascalCase`
    - Functions/variables: `camelCase` or `snake_case` – be consistent with surrounding code.
    - Macros: `UPPER_SNAKE_CASE` (avoid macros when possible).
- **Error handling**: Use `std::expected` or exceptions? The project currently uses custom `Error` hierarchy and exceptions for parsing. Follow the pattern in `parser.cpp`.
- **Internationalization**: All user‑facing strings must go through `Loc::msg<MsgId::...>` or `Loc::msgFormat<...>`.
- **Testing**: Add unit tests for new features in `tests/ic10/`.

### Pull Request Guidelines

- Target branch: `develop`.
- Include a clear description, linked issue if applicable.
- Ensure CI passes (build, test, static analysis).
- Keep changes focused – one PR per feature/bugfix.
- Update documentation (README, inline comments) if needed.

### Building with Extra Checks

- **Static analysis**:
  ```bash
  cppcheck --enable=all --suppress=missingIncludeSystem --std=c++23 -I common/include -I IC10/include src/
  ```
- **Address sanitizer** (Linux):
  ```bash
  cmake -B build-san -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address"
  ```

### Questions?

Feel free to open a discussion issue or contact the maintainer at [2207150234@st.sziit.edu.cn](mailto:2207150234@st.sziit.edu.cn).
