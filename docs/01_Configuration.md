[![logo](./logo.png)](#)

  # Configuration

All configuration properties can be set via a configuration file or via program parameters.

When determining the value of a single configuration property, the following precedence rules are applied - from highest to lowest priority:
1. Explicit Program Parameter  
A property specified directly as a command-line argument.
2. Configuration File  
If a configuration file is provided via the command-line parameter `--config=FILE`, its values are used.  
If no configuration file is provided, but a global configuration file is located at `~/.config/x11-overlayrc`, its values are used.
3. Default Values  
If nothing applies, fall back to default value.



A configuration file uses the [INI file format](https://en.wikipedia.org/wiki/INI_file) and could look like this:
```
## file that should be observed for changes and be displayed by x11-overlay
;InputFile=

[Positioning]
MonitorIndex=0
Orientation=NW
ScreenEdgeSpacing=0
LineSpacing=0

[Font]
Name=NotoSansMono
Size=12

[Colors]
AnsiProfile=VGA
ForegroundColor=[38;2;255;255;255m
ForegroundAlpha=255
BackgroundColor=[48;2;0;0;0m
BackgroundAlpha=100

[Behavior]
Dimming=0
MouseOverDimming=75
Tolerance=0
```
