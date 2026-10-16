# SAKK Developer Documentation

This document provides a technical overview of the SAKK chess application. It has been updated to reflect the modern C++ Object-Oriented architecture.

## Architecture Overview

The application is structured into several modules using strict Object-Oriented principles:

- **`main.cpp`**: The entry point. It defines the `Game` class, which acts as the main controller. It manages the state machine (menus, game loop, analysis mode) and directly owns the `BoardTree`.
- **`chess.cpp` / `chess.h`**: Contains the core chess logic. It features a polymorphic `Piece` hierarchy (abstract base `Piece` with `Pawn`, `Rook`, `Knight`, `Bishop`, `Queen`, `King` subclasses) for querying piece values and symbols. Move validation, castling, and check detection also live here.
- **`display.cpp` / `display.h`**: The `Display` static class handles all user interface and input operations. It abstracts the console drawing and input reading, communicating with the `econio` library.
- **`storage.cpp` / `storage.h`**: Defines the `Board` struct and the `BoardTree` class. The tree structure allows branching history (for Analysis Mode). The `BoardTree` fully manages memory allocation/deallocation via RAII.
- **`econio.c` / `econio.h`**: A custom university library for console I/O, providing functions for colored output and raw input reading.
- **`teszt.cpp`**: The test suite skeleton, demonstrating the instantiation of the polymorphic pieces and the construction of the board history tree.

---

## Piece Encoding Convention

The board state is internally represented as a `Piece*[8][8]` array inside the `Board` node. Each cell is either a pointer to a polymorphic `Piece` subclass or `nullptr` (empty square). When pieces are serialized to disk or displayed, they use character encoding:
**White pieces use lowercase letters, black pieces use uppercase letters.**

| Piece  | White (lowercase) | Black (uppercase) |
|--------|:-----------------:|:-----------------:|
| King   | `k`               | `K`               |
| Queen  | `q`               | `Q`               |
| Rook   | `r`               | `R`               |
| Bishop | `b`               | `B`               |
| Knight | `n`               | `N`               |
| Pawn   | `p`               | `P`               |
| Empty  | `.`               | `.`               |

The constant `SMALL_CAP_DISTANCE` (`32`) is the ASCII distance between a lowercase and its corresponding uppercase letter (`'a' - 'A' == 32`).

---

## Key Data Structures

### Polymorphic `Piece` (in `chess.h`)
The architecture uses polymorphism to determine piece properties without repetitive switch statements.
```cpp
class Piece {
protected:
  char m_figure;
  bool m_isWhite;
public:
  virtual char symbol() const = 0;
  virtual int value() const = 0;
  virtual ~Piece() = default;
};
```

### `Board` and `BoardTree` (in `storage.h`)
The game history is a tree to support multiple analytical branches.

```cpp
struct Board {
    Piece *board[8][8];
    Board *prev;
    int numNext;
    Board **next; // Dynamically allocated array for branching history
    int id;
    int previd;
    int selectedBranch; // Currently selected branch in analysis mode
    int en_passant_col; // Column where en passant capture is legal (-1 if none)
};
```
The `BoardTree` class completely encapsulates this structure. When a new branch is added, `addBoard()` expands the `next` pointer array using `new Board*[...]`. **Note: The project strictly avoids standard containers like `std::vector` to comply with university restrictions.** Memory is managed manually via `new[]` and `delete[]`.

### Move Validation Data (`Input`, `Booleans`, `Distances`)
These structs are passed by value/reference through the engine to simplify geometry logic (`is_rook_valid`, etc.) and to track castling legality (kings/rooks moved).

---

## Save/Load Serialization

The game state is saved by serializing the `Board` tree to a binary file using `std::ofstream` and `std::ifstream`.
Because pointers (`prev`, `next`) cannot be saved to disk, the serialization flattens the tree into a stream of `BoardElement` structs. The `id` and `previd` integers act as foreign keys.
- **Saving**: Traverses depth-first, writing each node's array and IDs.
- **Loading**: Reads elements sequentially, re-attaching them to their parent in memory based on the `previd`.

---

## Building and Development

To compile the C++ project, run:

```bash
g++ -std=c++17 -Wall storage.cpp chess.cpp display.cpp main.cpp econio.c -o sakk
```

To compile the NHF3 Skeleton Test instead of the game:
```bash
g++ -std=c++17 -Wall storage.cpp chess.cpp display.cpp teszt.cpp -o skeleton_test
```

### Memory Management
The legacy C `debugmalloc.h` has been **replaced by `memtrace`** (`memtrace.h` / `memtrace.cpp`), a university-provided memory leak detector that instruments `new` and `delete` calls at runtime. The architecture relies on standard C++ `new` and `delete`. The `Game` controller owns the `BoardTree`, and when it is destroyed or restarts, `BoardTree::~BoardTree()` recursively calls `freeAll()` to wipe the memory tree, preventing memory leaks automatically.
