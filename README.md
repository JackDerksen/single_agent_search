# CMPT 355 - Project 1: Single Agent Search (AB Solver)
### Jack Derksen, Rordon Crawley, Robin Huppertz, Tuan Huynh

## Features

- **Solvers**
  - **IDA\*** (default)
  - **A\*** (available in codebase)
- **Heuristics**
  - `ZeroHeuristic` (baseline)
  - `MisplacedDiscHeuristic`
  - `HopHeuristic` (precomputes hop distances)
- **Test inputs** included under `tests/` (e.g. `AB17`, `AB10`, etc.)

## Build

Use the provided Makefile:

```
make <optional: all/clean>
```

Binary output:

- macOS/Linux: `bin/AB`
- Windows (MinGW/MSYS2): `bin\AB.exe`

## Usage

### Solve a problem from stdin

Run with `<n>` equal to the number of disks/positions in the input vectors (the length of each line). The program expects:

1. `n` integers for the **large** disk layout
2. `n` integers for the **small** disk layout

Example (fast):

```
./bin/AB 17 < tests/AB17
```

This prints the sequence of states from start to the goal (inclusive)

### Debug mode (n = 0)

Passing `0` runs a hardcoded debug instance and enables solver debug output:

```
./bin/AB 0
```

## Notes

- By default, the program runs **IDA\*** and prints the solution path.
- The `tests/` directory contains sample instances you can redirect into stdin.
