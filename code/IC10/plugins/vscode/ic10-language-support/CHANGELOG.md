# Changelog

All notable changes to the "IC10" extension will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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
