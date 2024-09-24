# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

## [0.3.0] - 2024-09-24

### Added

- Make foreground text color configurable including alpha value for transparency.
- Make background color configurable including alpha value for transparency.
- Make general dimming configurable (and not only for mouse over behaviour) ([Issue #4](/../../issues/4)).
- Clean up help screen and introduce long option parameters for most options.
- Print version number via option parameter.

### Changed

- Rename configuration file section from "[MouseOver]" to "[Behaviour]".

## [0.2.0] - 2023-03-07

### Added

- Make font and font size configurable.

### Changed

- Rename configuration file section from "[Position]" to "[Positioning]".

### Fixed

- Fix compile error for ambiguous call to overloaded ‘sleep(int)’ function ([Issue #2](/../../issues/2)).

## [0.1.0] - 2021-09-09

### Added

- All configurations can be set via config file and overruled via command line parameters.
- Supports optional default configuration file located at `~/.config/x11-overlayrc`.
- Position overlay in all compass directions (N, NE, E, SE, S, SW, W, NW and CENTER).
- Position overlay on a specific monitor.
- Dimming overlay on mouse over by a given percentage value.
- Define mouse over tolerance by a given pixel value.
- Support XP and VGA color profile for ansi colors.
- Specify spacing between individual text lines and spacing to screen edges.
