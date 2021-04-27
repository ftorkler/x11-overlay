<h3 align="center">X11-Overlay</h3>
<p align="center">
<img alt="C++" src="https://img.shields.io/badge/C++-Solutions-blue.svg?logo=c%2B%2B"></img>
<img alt="Build Status" src="https://img.shields.io/github/workflow/status/ftorkler/x11-overlay/CI%20Build?logo=github">
</p>


# Installation

## Dependencies

debian/ubuntu:

```bash
$> apt install libx11-dev libxfixes-dev libxrandr-dev
```

arc/manjaro:

```bash
$> pacman -S libx11 libxfixes libxrandr
```

## Compile

```bash
$> make
```

# Usage

```bash
$> ./bin/overlay <FILE>
```