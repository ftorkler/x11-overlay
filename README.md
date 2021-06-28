<h3 align="center">X11-Overlay</h3>
<p align="center">
<img alt="C++" src="https://img.shields.io/badge/C++-Solutions-blue.svg?logo=c%2B%2B"></img>
<img alt="Build Status" src="https://img.shields.io/github/workflow/status/ftorkler/x11-overlay/CI%20Build?logo=github">
</p>

# Features

* screen selection for multi-monitor setup
* colorized text with Ansi Escape Sequences.  
Supported are the well known color formats for 3-4bit, 8bit and 24bit (see [SGR](https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters "Select_Graphic_Rendition")).

# Installation

## Dependencies

debian/ubuntu:

```
$> apt install libx11-dev libxfixes-dev libxrandr-dev libxft-dev libfreetype-dev
```

arc/manjaro:

```
$> pacman -S libx11 libxfixes libxrandr libxft freetype2
```

## Compile

```
$> make && ./bin/run_tests
```

# Usage

```
usage: overlay [OPTIONS] <INPUT_FILE>

OPTIONS:
  -o <value>          orientation to align window and lines; defaults to 'NW'
                      possible values are N, NE, E, SE, S, SW, W, NW and CENTER
  -d <percent>        how much the window dims on mouse over; defaults to '75'%
  -t <pixel>          tolerance in pixel for mouse over dimming; defaults to '0'
  -s <pixel>          screen edge spacing in pixels; defaults to '0'
  -l <pixel>          line spacing in pixels; defaults to '0'
  -h                  prints this help text
```