# XRoar Trace Profiler

A CPU cycle profiling toolset for 6809 assembly code running in the [XRoar](http://www.6809.org.uk/xroar/) Dragon/CoCo emulator.

## Overview

This toolset allows you to profile 6809 code execution by:

1. Capturing instruction traces from XRoar with cycle timing
2. Processing traces into a profile file with the `xrtprof` filter
3. Visualizing the profile with an interactive HTML viewer

The viewer shows which instructions consumed the most CPU cycles, making it easy to identify performance hotspots in 6809 assembly code.

**Limitation:** This profiler does not currently support MMU-equipped machines with physical address space extending beyond 64K (e.g., CoCo 3).

## Quick Start

### 1. Generate a Trace

Run XRoar with trace output piped directly through the filter:

```bash
xroar -q -trace -trace-timing | xrtprof > profile.json
```

**Note:** Trace files can become extraordinarily large. Piping output directly through the filter is strongly recommended for anything other than exceptionally short traces.

For short traces, you can save the raw trace first, but consider using strategies to limit trace file size - start with tracing inactive and toggle tracing on/off during execution with keyboard commands or the XRoar `-trap` command-line options.

```bash
xroar -q -trace-timing > trace.txt
xrtprof trace.txt profile.json
```

**XRoar flags:**

- `-q`: Quiet mode (suppresses other emulator informational output)
- `-trace`: Enable instruction tracing at emulator start
- `-trace-timing`: Include cycle timing information

Consult XRoar documentation for command line options for trap configuration to toggle tracing on and off.

### 2. View the Profile

Open `xrpview.html` in a web browser and either:

- Click **LOAD** and select your `profile.json` file
- Drag and drop the JSON file onto the viewer

## Building the Filter

The `xrtprof` filter is a simple C program that can be built natively on Linux or cross-compiled for Windows using MinGW.

### Prerequisites

**Linux:**

- GCC compiler
- Standard C library

**Windows (cross-compile):**

- MinGW-w64 cross-compiler (`x86_64-w64-mingw32-gcc`)
- Recommended: Use WSL (Windows Subsystem for Linux) to host the build environment

### Building

**Linux native:**

```bash
make linux
# Binary will be in build/linux/xrtprof
```

**Windows (cross-compile from Linux/WSL):**

```bash
make windows
# Binary will be in build/windows/xrtprof.exe
```

**Both platforms:**

```bash
make all
```

**Debug build:**

```bash
make debug
# Creates xrtprof_g in the current directory with debug symbols
```

### Installing MinGW on Ubuntu/Debian

```bash
sudo apt install mingw-w64
```

The Makefile uses `x86_64-w64-mingw32-gcc` for Windows builds, which produces 64-bit executables.

## Viewer Features

  ## Screenshots ![XRoar Trace Profiler Viewer](C:\Users\44798\Sync\Projects\C\xrtprof\repo\screenshots\viewer.png)

### Navigation

- **Scroll wheel**: Zoom in/out on the graph
- **Drag**: Pan left/right through address space
- **Double-click**: Select instruction (graph or table)

### Zoom Presets

- **64K**: View entire address space
- **VECT**: Jump to interrupt vectors ($FFF0-$FFFF)
- **ALL**: Zoom to all traced instructions
- **LO32/HI32**: Focus on lower/upper 32K

### Search

- **Ctrl+F**: Focus search box
- **F3**: Find next match
- **Shift+F3**: Find previous match
- **WRAP**: Continue from start/end when reaching end/start

### Display Modes

- **LOG Y**: Toggle logarithmic Y-axis for better visualization of low-cycle instructions

### Highlighting

- **Blue**: Selected instruction (persistent)
- **Green**: Hovered instruction (temporary)
- Highlights are synchronized between graph and table

## Understanding the Profile

### Graph

- **X-axis**: Memory address ($0000-$FFFF)
- **Y-axis**: Traced instruction cycles (total cycles spent at each address)
- **Bars**: Height indicates CPU time spent executing instructions at that address

### Table Columns

- **ADDRESS**: Instruction location in memory
- **TRACED INSTRUCTION CYCLES**: Total cycles spent executing this instruction
- **INSTRUCTION**: Disassembly with cycle count per execution

### Summary Statistics

- **TOTAL INSTRUCTIONS TRACED**: Number of unique instruction addresses
- **INSTRUCTION EXECUTIONS**: Total number of instructions executed
- **CPU CYCLES**: Sum of all instruction cycles
- **TRACE TIME**: Estimated execution time at 0.89 MHz (Dragon 32/64)

## Profile Format

The JSON profile format:

```json
[
  {
    "a": 48072,
    "ic": 6415670,
    "ac": 3207835.00,
    "ins": "LEAX  -1,X               ; (5 cycles)"
  }
]
```

- **a**: Address
- **ic**: Instruction cycles (total cycles this instruction was executed)
- **ac**: Address cycles (cycles attributed to this specific address byte)
- **ins**: Disassembly with cycle timing

#### Windows Usage Notes

XRoar on Windows must be run from CMD, PowerShell or a Shortcut to enable piping.

### Using a Windows Shortcut (Recommended)

Create a shortcut with the following target configuration:

```
%windir%\system32\cmd.exe /k ""C:\Program Files\XRoar\xroar.exe" -q -trace-timing 2>&1 | "C:\Program Files\XRoar\xrtprof.exe" > profile.json"
```

Adjust the paths to match your XRoar and xrtprof installation locations. The `/k` flag keeps the command window open so you can see any errors.

### Command Line Execution

Alternatively, run directly from CMD or PowerShell:

```cmd
xroar.exe -q -trace -trace-timing 2>&1 | xrtprof.exe > profile.json
```

**Note:** The `2>&1` redirection may be necessary to ensure trace output is available to the pipe. Do not use XRoar's `-C` option for console output when profiling.

### Alternative: File-based Workflow

If piping doesn't work:

```cmd
xroar.exe -q -trace -trace-timing 2>&1 > trace.txt
xrtprof.exe trace.txt profile.json
```

## License

Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)

Copyright (c) 2025 Mike Miller

This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

You are free to:

- Share — copy and redistribute the material in any medium or format
- Adapt — remix, transform, and build upon the material

Under the following terms:

- Attribution — You must give appropriate credit
- NonCommercial — You may not use the material for commercial purposes
- ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license

See https://creativecommons.org/licenses/by-nc-sa/4.0/ for full license text.

## Author

Mike Miller (bluearcus)

mike.miller@gmx.co.uk


















