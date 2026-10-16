![chess-tui](https://github.com/user-attachments/assets/ba211d1b-e9bf-49ab-867f-84d1ecf9bc6b)
# Hungarian Console Chess Program in C++

This repository contains a fully featured, character-based (TUI - Terminal User Interface) chess program written in C++. The program offers interactive two-player gameplay, supports a game analysis mode with branching move trees, and saves game states to disk.

## 📌 Features

- **Full chess logic:** Validates legal moves for all pieces, including captures, check, checkmate, stalemate, castling, en passant, and pawn promotion.
- **Object-oriented architecture:** Uses a polymorphic `Piece` hierarchy (`Pawn`, `Rook`, `Knight`, `Bishop`, `Queen`, `King`) with virtual dispatch for move validation, symbols, and values.
- **Colorful terminal interface:** Uses the `econio` library to provide a colorful console display with board visualization.
- **Game analysis mode:** Load a saved game and replay it move by move. Supports branching variations — you can explore alternative lines from any position.
- **Save and load games:** Finished or in-progress games can be saved to the `Games` folder and loaded later.
- **Memory-safe operation:** The project uses `memtrace` for memory leak detection during development.

## 🎮 How to Play

Moves are entered as a 4-character string representing the source and destination squares:

```
<column><row><column><row>   →   e.g. E2E4 (move piece from E2 to E4)
```

- Columns are letters `A`–`H`, rows are numbers `1`–`8`, both uppercase.
- The program will notify you if a move is illegal.

### Analysis Mode Controls

| Key       | Action                              |
|-----------|-------------------------------------|
| `←` / `→` | Step backward / forward in the game |
| `1`       | Jump to starting position           |
| `2`       | Jump to the last move               |
| `3`       | Jump to a specific move number      |
| `4`       | Create a new variation from here    |
| `5`       | Switch to a different branch        |
| `X`       | Exit analysis mode                  |

> **Note:** The game file browser in analysis mode is currently Windows-only.

## 🏗️ Architecture and File Structure

The source code is modular, following Object-Oriented principles with a polymorphic piece hierarchy and an MVC-inspired separation of concerns:

- **`main.cpp`**: The entry point containing the main game loop and the `Game` controller logic. It manages the state machine (menus, game loop, analysis mode) and owns the `BoardTree`.
- **`chess.cpp` / `chess.h`**: The core of the game (Engine / Model). Contains the polymorphic `Piece` hierarchy and handles move validation, castling, check/checkmate detection, and board evaluation.
- **`display.cpp` / `display.h`**: The module responsible for visual presentation (View). The `Display` static class draws the board, pieces, menus, and handles user input via `econio`.
- **`storage.cpp` / `storage.h`**: The file management module. Defines `Board` and `BoardTree` — a tree structure for branching move history. Handles save/load serialization to disk.
- **`econio.c` / `econio.h`**: A university library for console terminal manipulation (cursor movement, colors, detecting immediate keystrokes). Written in C with an `extern "C"` wrapper for C++ compatibility.
- **`teszt.cpp`**: The test suite using `gtest_lite.h`, covering piece instantiation, move validation, check/checkmate detection, and board tree operations.
- **`gtest_lite.h`**: A lightweight test framework header (university-provided).
- **`memtrace.cpp` / `memtrace.h`**: A memory leak detection tool that tracks `new`/`delete` calls at runtime.

---

## 🚀 Installation and Compilation

To compile the program, you need a C++ compiler (e.g., g++ or clang++).

### Compiling from the command line

Since the project consists of multiple source files, all `.cpp` and `.c` files must be included in the compilation. Open a terminal in the root folder and run:

```bash
g++ -std=c++17 -Wall storage.cpp chess.cpp display.cpp main.cpp memtrace.cpp econio.c -o chess
```

To compile the test suite instead of the game:

```bash
g++ -std=c++17 -Wall storage.cpp chess.cpp display.cpp teszt.cpp memtrace.cpp econio.c -o teszt
```

For more details on the internal architecture, see [DEVELOPER.md](DEVELOPER.md) (also available in [Hungarian](DEVELOPER.hu.md)).
