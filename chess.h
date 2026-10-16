/*
Tóth Avar Áron
DQMKMR
*/

#ifndef CHESS_H
#define CHESS_H
#include <cstdlib>

const int SMALL_CAP_DISTANCE = 32;

const int VAL_QUEEN = 9;
const int VAL_ROOK = 5;
const int VAL_KNIGHT = 3;
const int VAL_BISHOP = 3;
const int VAL_PAWN = 1;
const int VAL_KING = 0;

struct BoolTable;

class Piece {
protected:
  char m_figure;
  bool m_isWhite;

public:
  Piece(char figure, bool white) : m_figure(figure), m_isWhite(white) {}

  virtual char symbol() const = 0;
  virtual int value() const = 0;

  virtual bool isValidMove(int origRow, int origCol, int targetRow,
                           int targetCol, Piece *board[8][8],
                           const BoolTable &b, int enPassantCol) const = 0;

  virtual Piece *clone() const = 0;

  virtual ~Piece() {}
};

class Pawn : public Piece {
public:
  explicit Pawn(bool white = false) : Piece('P', white) {}
  char symbol() const override { return m_isWhite ? 'p' : 'P'; }
  int value() const override { return VAL_PAWN; }
  bool isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                   Piece *board[8][8], const BoolTable &b,
                   int enPassantCol) const override;
  Piece *clone() const override { return new Pawn(m_isWhite); }
};

class Rook : public Piece {
public:
  explicit Rook(bool white = false) : Piece('R', white) {}
  char symbol() const override { return m_isWhite ? 'r' : 'R'; }
  int value() const override { return VAL_ROOK; }
  bool isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                   Piece *board[8][8], const BoolTable &b,
                   int enPassantCol) const override;
  Piece *clone() const override { return new Rook(m_isWhite); }
};

class Knight : public Piece {
public:
  explicit Knight(bool white = false) : Piece('N', white) {}
  char symbol() const override { return m_isWhite ? 'n' : 'N'; }
  int value() const override { return VAL_KNIGHT; }
  bool isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                   Piece *board[8][8], const BoolTable &b,
                   int enPassantCol) const override;
  Piece *clone() const override { return new Knight(m_isWhite); }
};

class Bishop : public Piece {
public:
  explicit Bishop(bool white = false) : Piece('B', white) {}
  char symbol() const override { return m_isWhite ? 'b' : 'B'; }
  int value() const override { return VAL_BISHOP; }
  bool isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                   Piece *board[8][8], const BoolTable &b,
                   int enPassantCol) const override;
  Piece *clone() const override { return new Bishop(m_isWhite); }
};

class Queen : public Piece {
public:
  explicit Queen(bool white = false) : Piece('Q', white) {}
  char symbol() const override { return m_isWhite ? 'q' : 'Q'; }
  int value() const override { return VAL_QUEEN; }
  bool isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                   Piece *board[8][8], const BoolTable &b,
                   int enPassantCol) const override;
  Piece *clone() const override { return new Queen(m_isWhite); }
};

class King : public Piece {
public:
  explicit King(bool white = false) : Piece('K', white) {}
  char symbol() const override { return m_isWhite ? 'k' : 'K'; }
  int value() const override { return VAL_KING; }
  bool isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                   Piece *board[8][8], const BoolTable &b,
                   int enPassantCol) const override;
  Piece *clone() const override { return new King(m_isWhite); }
};

Piece *pieceFromSymbol(char sym);

struct Input {
  int orig_row;
  int orig_column;
  int target_row;
  int target_column;
  char figure;
  bool capture;
  bool white;

  Input()
      : orig_row(0), orig_column(0), target_row(0), target_column(0),
        figure('\0'), capture(false), white(false) {}

  Input(int or_, int oc, int tr, int tc, char fig, bool cap, bool wh)
      : orig_row(or_), orig_column(oc), target_row(tr), target_column(tc),
        figure(fig), capture(cap), white(wh) {}
};

struct BoolTable {
  bool check;
  bool mate;
  bool stalemate;
  bool isWhiteTurn;
  bool white_king_moved;
  bool black_king_moved;
  bool white_rook_A_moved;
  bool white_rook_H_moved;
  bool black_rook_A_moved;
  bool black_rook_H_moved;
  bool playing;

  BoolTable()
      : check(false), mate(false), stalemate(false), isWhiteTurn(true),
        white_king_moved(false), black_king_moved(false),
        white_rook_A_moved(false), white_rook_H_moved(false),
        black_rook_A_moved(false), black_rook_H_moved(false), playing(true) {}

  BoolTable(bool chk, bool mt, bool sm, bool whiteTurn, bool play = true)
      : check(chk), mate(mt), stalemate(sm), isWhiteTurn(whiteTurn),
        white_king_moved(false), black_king_moved(false),
        white_rook_A_moved(false), white_rook_H_moved(false),
        black_rook_A_moved(false), black_rook_H_moved(false), playing(play) {}
};

typedef BoolTable Booleans;

struct Distances {
  int row_distance;
  bool row_distance_pos;
  int column_distance;
  bool column_distance_pos;

  Distances()
      : row_distance(0), row_distance_pos(false), column_distance(0),
        column_distance_pos(false) {}

  Distances(int rowDist, bool rowPos, int colDist, bool colPos)
      : row_distance(rowDist), row_distance_pos(rowPos),
        column_distance(colDist), column_distance_pos(colPos) {}
};

void copy_board(Piece *src[8][8], Piece *dst[8][8]);
void free_board(Piece *board[8][8]);
void init_board(Piece *board[8][8]);

Input curr_move(char move[5], Piece *position[8][8]);

void bool_checker(Piece *position[8][8], BoolTable *b);

bool is_king_in_check(Piece *position[8][8], bool check_WhiteKing);

bool is_move_pattern_valid(const Input &move, Piece *position[8][8],
                           bool isWhiteTurn, bool modifyBoard,
                           const BoolTable &b, int en_passant_col,
                           int *next_en_passant_col);

bool any_valid_moves(Piece *position[8][8], bool isWhiteTurn,
                     int en_passant_col);

bool pawn_promotion(const Input &move);

void reconstruct_move(Piece *board_now[8][8], Piece *board_prev[8][8],
                      char *move_buffer);

void do_promotion(const Input &move, Piece *position[8][8], char figure);

void format_chess_notation(const Input &move, char *buffer);

int check_eval(Piece *board[8][8]);

#endif // CHESS_H