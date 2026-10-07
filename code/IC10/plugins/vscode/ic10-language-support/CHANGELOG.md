# Changelog

All notable changes to the "IC10" extension will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [2.2.0] - 2026-10-07

### Added
- Search device types by name: the `IC10: Search Device Type` command (`ic10.searchDeviceType`, `Ctrl+Alt+Shift+R`) opens a quick pick that fuzzy-matches the localized Chinese or English name and inserts the device type name
- Completion for the value of `#: @type`: device type names (enumeration types are deliberately not offered), listed by their localized name with the English name beside it, and always inserting the type name
- Completion inside `HASH("…")`: the model names the game expects (raw model names, parentheses included), and typing the quote itself now opens the list
- Pseudocode inlay hints: a readable rendering at the end of every instruction line (`add a a 1` → `a = a + 1`, `beq a 10 5` → `if a == 10 goto 5`), left-aligned against the longest instruction line of the file, with a branch's multi-line form in the hint's tooltip. The templates cover all 147 instructions of the standard library, an unfinished instruction shows nothing, and the hints are hidden until `Ctrl+Alt` (`Ctrl+Option` on macOS) is held; IC10's hint label length limit is also lifted so the aligned labels are not truncated

### Changed
- Device and model names are shown as "localized name + English name". The second name stays English, so the lists cannot become cluttered as more languages are added (with an English UI the two coincide and only one is shown)
- The language client is awaited during activation, so the language features are registered before activation completes and a language server that fails to start fails loudly instead of going silent
- `@ic10/metadata` 1.1.0: localized device titles and descriptions, used by the new name search and completion

### Fixed
- Hover, completion, formatting and semantic highlighting: a label whose identifier failed to parse no longer breaks the feature (the AST types now say that the identifier can be an error node)
- Completion details for device members were always empty: the code read a `tag` field that annotation lines never carry

## [2.1.0] - 2026-10-03

### Added
- Debugger: launch IC10 programs with breakpoints, stepping, the call stack, and a variables panel grouped by devices / registers / labels
- Device-member defaults through the `#: @default <category> <field> <value>` tag (a register or alias writes `@default <value>`), so the variables panel shows a declared default instead of "Unable to evaluate" for fields the script never assigns
- Type narrowing for completion: `Ctrl+Alt+D/R/N/E/I/K` narrows the list to devices / registers / numbers / enums / identifiers / keywords, and `Ctrl+Alt+0` restores it
- Hover cards for devices, aliases, `define` constants, labels, enumeration members, and type-hint tags: a type-colored icon, the localized type name, column-aligned fields, and the current file path as a footnote
- `ic10.hover.maxWidth` to cap the width of a hover card
- Localized extension manifest (`package.nls.json` / `package.nls.zh-cn.json`) for the English and Chinese UI

### Changed
- Hover: the first line is now the icon plus the localized type name, and the second line is `(kind) name: EnglishType = literal`; for `HASH`/`STR` the `=` slot keeps the literal and the runtime-computed number moves to a value field
- Runtime options (`ic10.runtime.*`) are filled into launch configurations that omit them
- Standard library: device annotations now cover the full device list generated from the game metadata (type and name hashes, logic and slot-logic members, slots) — shipped with the `@ic10/compiler` dependency

### Fixed
- Correct misspelled `LogicType` members in the standard library and the enum metadata (`RatioLquid*` / `Ratiol.Iquid*`, `Volatiles`, `StackTrace`, `SemMajorAxis`, and the `true` member a YAML round-trip had mangled) so they match the game's own enum table

## [2.0.0] - 2026-09-26

### Added
- Integrate the IC10 runtime (`ic10r-node`) into the language server
- Add a hover renderer that can be switched between SVG and Markdown

### Changed
- Adapt the extension to IC10 v3 (`@ic10/compiler` v3.1.0)
- Restructure the extension into a pnpm workspace of `client` / `common` / `debugger` / `server` packages
- Extract instructions, enums and Stationpedia data into the standalone `@ic10/metadata` package
- Ship `client` and `server` as esbuild bundles, so the installed extension no longer depends on the `node_modules` layout

### Fixed
- Patch the metadata and standard library data

## [1.0.2] - 2026-07-31

- Fix invalid images in README when displayed in Marketplace

## [1.0.1] - 2026-07-31

- Attempt to fix invalid images in README when displayed in Marketplace

## [1.0.0] - 2026-07-31

- publish release 1.0.0

## [0.5.0] - 2026-07-31

### Fixed
- Fix plugin errors in release build
- Fix escape error in string handling

## [0.4.0] - 2026-07-30

### Added
- Enhance error recovery for parser
- Supplement LSP configuration

### Changed
- Improve plugin documentation
- Optimize plugin performance

### Fixed
- Packaging correction

## [0.3.0] - 2026-07-28

### Added
- Implement IC10 LSP signature help function
- Implement IC10 LSP formatting function

## [0.2.0] - 2026-07-27

### Added
- Implement IC10 LSP completion function
- Export type table for compiler integration

## [0.1.0] - 2026-07-25

### Added
- Implement IC10 LSP hover function
- Implement IC10 LSP semantic tokens
- Implement IC10 LSP diagnostic report function
- LSP hover update

## [0.0.0] - 2026-06-29

### Added
- Initial release
- LSP (Language Server Protocol) support for IC10 language
- Hover prompt function
- Basic diagnostics support
