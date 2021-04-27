<h2 align="center">X11-Overlay</h3>
<p align="center">
<img src="https://img.shields.io/badge/C++-Solutions-blue.svg?logo=c%2B%2B"></img>
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