/*
Tóth Avar Áron
DQMKMR
*/

#include "chess.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "memtrace.h"

Piece *pieceFromSymbol(char sym) {
  bool white = (sym >= 'a' && sym <= 'z');
  char upper = white ? (char)(sym - 32) : sym;
  switch (upper) {
  case 'P':
    return new Pawn(white);
  case 'R':
    return new Rook(white);
  case 'N':
    return new Knight(white);
  case 'B':
    return new Bishop(white);
  case 'Q':
    return new Queen(white);
  case 'K':
    return new King(white);
  default:
    return nullptr;
  }
}

void copy_board(Piece *src[8][8], Piece *dst[8][8]) {
  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++)
      dst[r][c] = (src[r][c] != nullptr) ? src[r][c]->clone() : nullptr;
}

void free_board(Piece *board[8][8]) {
  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++) {
      delete board[r][c];
      board[r][c] = nullptr;
    }
}

void init_board(Piece *board[8][8]) {

  const char startPos[8][8] = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                               {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                               {'.', '.', '.', '.', '.', '.', '.', '.'},
                               {'.', '.', '.', '.', '.', '.', '.', '.'},
                               {'.', '.', '.', '.', '.', '.', '.', '.'},
                               {'.', '.', '.', '.', '.', '.', '.', '.'},
                               {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                               {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};
  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++)
      board[r][c] =
          (startPos[r][c] != '.') ? pieceFromSymbol(startPos[r][c]) : nullptr;
}

static inline bool isWhitePiece(const Piece *p) {
  if (p == nullptr)
    return false;
  char s = p->symbol();
  return (s >= 'a' && s <= 'z');
}

static bool isFriendlyFire(const Input &move, Piece *position[8][8]) {
  Piece *target = position[move.target_row][move.target_column];
  if (target == nullptr)
    return false;
  return (move.white == isWhitePiece(target));
}

static bool isPathFree(const Input &move, Piece *position[8][8],
                       const Distances &d) {
  int rDir = d.row_distance_pos ? 1 : (d.row_distance > 0 ? -1 : 0);
  int cDir = d.column_distance_pos ? 1 : (d.column_distance > 0 ? -1 : 0);

  if (d.row_distance > 0 && d.column_distance == 0) {
    for (int i = 1; i < d.row_distance; i++)
      if (position[move.orig_row + i * rDir][move.orig_column] != nullptr)
        return false;
  } else if (d.column_distance > 0 && d.row_distance == 0) {
    for (int i = 1; i < d.column_distance; i++)
      if (position[move.orig_row][move.orig_column + i * cDir] != nullptr)
        return false;
  } else if (d.row_distance == d.column_distance && d.row_distance > 0) {
    for (int i = 1; i < d.row_distance; i++)
      if (position[move.orig_row + i * rDir][move.orig_column + i * cDir] !=
          nullptr)
        return false;
  }
  return true;
}

static void setupDistances(const Input &move, Distances &d) {
  d.row_distance = abs(move.target_row - move.orig_row);
  d.row_distance_pos = (move.target_row > move.orig_row);
  d.column_distance = abs(move.target_column - move.orig_column);
  d.column_distance_pos = (move.target_column > move.orig_column);
}

bool Pawn::isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                       Piece *board[8][8], const BoolTable &,
                       int enPassantCol) const {
  int rowDiff = targetRow - origRow;
  int colDiff = abs(targetCol - origCol);
  int absRowDiff = abs(rowDiff);

  if (m_isWhite && rowDiff <= 0)
    return false;
  if (!m_isWhite && rowDiff >= 0)
    return false;

  Piece *target = board[targetRow][targetCol];

  if (colDiff == 0) {
    if (target != nullptr)
      return false;
    if (absRowDiff == 1)
      return true;
    if (absRowDiff == 2) {
      int midRow = (origRow + targetRow) / 2;
      if (m_isWhite && origRow == 1 && board[midRow][origCol] == nullptr)
        return true;
      if (!m_isWhite && origRow == 6 && board[midRow][origCol] == nullptr)
        return true;
    }
  } else if (colDiff == 1 && absRowDiff == 1) {
    if (target != nullptr)
      return true;
    if (targetCol == enPassantCol) {
      if (m_isWhite && origRow == 4)
        return true;
      if (!m_isWhite && origRow == 3)
        return true;
    }
  }
  return false;
}

bool Rook::isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                       Piece *board[8][8], const BoolTable &, int) const {
  if (origRow != targetRow && origCol != targetCol)
    return false;
  Input m;
  m.orig_row = origRow;
  m.orig_column = origCol;
  m.target_row = targetRow;
  m.target_column = targetCol;
  Distances d;
  setupDistances(m, d);
  return isPathFree(m, board, d);
}

bool Knight::isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                         Piece *[8][8], const BoolTable &, int) const {
  int rD = abs(targetRow - origRow);
  int cD = abs(targetCol - origCol);
  return (rD == 1 && cD == 2) || (rD == 2 && cD == 1);
}

bool Bishop::isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                         Piece *board[8][8], const BoolTable &, int) const {
  int rD = abs(targetRow - origRow);
  int cD = abs(targetCol - origCol);
  if (rD != cD || rD == 0)
    return false;
  Input m;
  m.orig_row = origRow;
  m.orig_column = origCol;
  m.target_row = targetRow;
  m.target_column = targetCol;
  Distances d;
  setupDistances(m, d);
  return isPathFree(m, board, d);
}

bool Queen::isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                        Piece *board[8][8], const BoolTable &b,
                        int enPassantCol) const {
  int rD = abs(targetRow - origRow);
  int cD = abs(targetCol - origCol);
  bool rookMove = (origRow == targetRow || origCol == targetCol);
  bool bishopMove = (rD == cD && rD > 0);
  if (!rookMove && !bishopMove)
    return false;

  if (rookMove)
    return Rook(m_isWhite).isValidMove(origRow, origCol, targetRow, targetCol,
                                       board, b, enPassantCol);
  return Bishop(m_isWhite).isValidMove(origRow, origCol, targetRow, targetCol,
                                       board, b, enPassantCol);
}

bool King::isValidMove(int origRow, int origCol, int targetRow, int targetCol,
                       Piece *[8][8], const BoolTable &, int) const {
  int rD = abs(targetRow - origRow);
  int cD = abs(targetCol - origCol);
  return (rD <= 1 && cD <= 1 && (rD + cD) > 0);
}

Input curr_move(char move[5], Piece *position[8][8]) {
  Input result;

  char c0 = (move[0] >= 'a' && move[0] <= 'z') ? (char)(move[0] - 32) : move[0];
  char c2 = (move[2] >= 'a' && move[2] <= 'z') ? (char)(move[2] - 32) : move[2];

  result.orig_column = c0 - 'A';
  result.orig_row = move[1] - '0' - 1;
  result.target_column = c2 - 'A';
  result.target_row = move[3] - '0' - 1;

  if (result.orig_column < 0 || result.orig_column > 7 || result.orig_row < 0 ||
      result.orig_row > 7 || result.target_column < 0 ||
      result.target_column > 7 || result.target_row < 0 ||
      result.target_row > 7) {
    return Input();
  }

  Piece *p = position[result.orig_row][result.orig_column];
  if (p != nullptr) {
    char sym = p->symbol();
    result.white = (sym >= 'a' && sym <= 'z');
    result.figure = result.white ? (char)(sym - 32) : sym;
  }
  result.capture =
      (position[result.target_row][result.target_column] != nullptr);
  return result;
}

void bool_checker(Piece *position[8][8], BoolTable *b) {
  auto check = [&](Piece *p, char expected, bool &flag) {
    if (p == nullptr || p->symbol() != expected)
      flag = true;
  };
  check(position[0][0], 'r', b->white_rook_A_moved);
  check(position[0][7], 'r', b->white_rook_H_moved);
  check(position[7][0], 'R', b->black_rook_A_moved);
  check(position[7][7], 'R', b->black_rook_H_moved);
  check(position[0][4], 'k', b->white_king_moved);
  check(position[7][4], 'K', b->black_king_moved);
}

bool is_king_in_check(Piece *position[8][8], bool check_WhiteKing) {
  int king_row = -1, king_col = -1;
  char kingSym = check_WhiteKing ? 'k' : 'K';

  for (int r = 0; r < 8 && king_row == -1; r++)
    for (int c = 0; c < 8; c++)
      if (position[r][c] != nullptr && position[r][c]->symbol() == kingSym) {
        king_row = r;
        king_col = c;
        break;
      }
  if (king_row == -1)
    return false;

  BoolTable dummy;
  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++) {
      Piece *p = position[r][c];
      if (p == nullptr)
        continue;
      if (isWhitePiece(p) == check_WhiteKing)
        continue;
      if (p->isValidMove(r, c, king_row, king_col, position, dummy, -1))
        return true;
    }
  return false;
}

static bool isCastlingValid(const Input &move, Piece *position[8][8],
                            const Distances &d, const BoolTable &b) {
  if (move.white && b.white_king_moved)
    return false;
  if (!move.white && b.black_king_moved)
    return false;
  if (!(d.row_distance == 0 && d.column_distance == 2))
    return false;

  if (!isPathFree(move, position, d))
    return false;

  bool validDistances = false;
  bool queenside = false;

  if (move.white) {
    if (move.target_column == 2 && !b.white_rook_A_moved) {
      validDistances = true;
      queenside = true;
    }
    if (move.target_column == 6 && !b.white_rook_H_moved)
      validDistances = true;
  } else {
    if (move.target_column == 2 && !b.black_rook_A_moved) {
      validDistances = true;
      queenside = true;
    }
    if (move.target_column == 6 && !b.black_rook_H_moved)
      validDistances = true;
  }
  if (!validDistances)
    return false;

  if (position[move.orig_row][move.target_column] != nullptr)
    return false;
  if (queenside && position[move.orig_row][1] != nullptr)
    return false;

  int colDir = d.column_distance_pos ? 1 : -1;
  for (int i = 0; i <= 2; i++) {
    Piece *temp[8][8];
    copy_board(position, temp);

    delete temp[move.orig_row][move.orig_column];
    temp[move.orig_row][move.orig_column] = nullptr;

    int col = move.orig_column + i * colDir;
    temp[move.orig_row][col] = new King(move.white);
    bool inCheck = is_king_in_check(temp, move.white);
    free_board(temp);
    if (inCheck)
      return false;
  }
  return true;
}

bool is_move_pattern_valid(const Input &move, Piece *position[8][8],
                           bool isWhiteTurn, bool modifyBoard,
                           const BoolTable &b, int en_passant_col,
                           int *next_en_passant_col) {
  if (move.orig_row == move.target_row &&
      move.orig_column == move.target_column)
    return false;

  if (isWhiteTurn != move.white)
    return false;
  if (isFriendlyFire(move, position))
    return false;

  Piece *p = position[move.orig_row][move.orig_column];
  if (p == nullptr)
    return false;

  Distances d;
  setupDistances(move, d);

  bool geometryValid =
      p->isValidMove(move.orig_row, move.orig_column, move.target_row,
                     move.target_column, position, b, en_passant_col);

  if (!geometryValid && move.figure == 'K')
    if (isCastlingValid(move, position, d, b))
      geometryValid = true;

  if (!geometryValid)
    return false;

  Piece *temp[8][8];
  copy_board(position, temp);

  delete temp[move.target_row][move.target_column];
  temp[move.target_row][move.target_column] =
      temp[move.orig_row][move.orig_column];
  temp[move.orig_row][move.orig_column] = nullptr;

  if (move.figure == 'P' && move.target_column == en_passant_col &&
      position[move.target_row][move.target_column] == nullptr) {
    int capturedRow = move.white ? move.target_row - 1 : move.target_row + 1;
    delete temp[capturedRow][move.target_column];
    temp[capturedRow][move.target_column] = nullptr;
  }

  bool stillInCheck = is_king_in_check(temp, isWhiteTurn);
  free_board(temp);
  if (stillInCheck)
    return false;

  if (modifyBoard) {

    if (move.figure == 'K' && d.column_distance == 2) {
      int r = move.orig_row;
      bool shortCastle = d.column_distance_pos;
      if (shortCastle) {
        position[r][5] = position[r][7];
        position[r][7] = nullptr;
      } else {
        position[r][3] = position[r][0];
        position[r][0] = nullptr;
      }
    }

    if (move.figure == 'P' && move.target_column == en_passant_col &&
        position[move.target_row][move.target_column] == nullptr) {
      int capturedRow = move.white ? move.target_row - 1 : move.target_row + 1;
      delete position[capturedRow][move.target_column];
      position[capturedRow][move.target_column] = nullptr;
    }

    delete position[move.target_row][move.target_column];
    position[move.target_row][move.target_column] =
        position[move.orig_row][move.orig_column];
    position[move.orig_row][move.orig_column] = nullptr;

    if (next_en_passant_col != nullptr) {
      if (move.figure == 'P' && abs(move.target_row - move.orig_row) == 2)
        *next_en_passant_col = move.target_column;
      else
        *next_en_passant_col = -1;
    }
  }
  return true;
}

bool any_valid_moves(Piece *position[8][8], bool isWhiteTurn,
                     int en_passant_col) {
  BoolTable b;
  for (int r1 = 0; r1 < 8; r1++) {
    for (int c1 = 0; c1 < 8; c1++) {
      Piece *p = position[r1][c1];
      if (p == nullptr)
        continue;
      if (isWhitePiece(p) != isWhiteTurn)
        continue;

      char sym = p->symbol();
      char fig = (sym >= 'a') ? (char)(sym - 32) : sym;

      for (int r2 = 0; r2 < 8; r2++) {
        for (int c2 = 0; c2 < 8; c2++) {
          if (r1 == r2 && c1 == c2)
            continue;
          bool isCapture = (position[r2][c2] != nullptr);

          if (fig == 'P' && c2 == en_passant_col) {
            if (isWhiteTurn && r1 == 4 && r2 == 5)
              isCapture = true;
            if (!isWhiteTurn && r1 == 3 && r2 == 2)
              isCapture = true;
          }
          Input test(r1, c1, r2, c2, fig, isCapture, isWhiteTurn);
          if (is_move_pattern_valid(test, position, isWhiteTurn, false, b,
                                    en_passant_col, nullptr))
            return true;
        }
      }
    }
  }
  return false;
}

bool pawn_promotion(const Input &move) {
  return move.figure == 'P' && ((move.white && move.target_row == 7) ||
                                (!move.white && move.target_row == 0));
}

void do_promotion(const Input &move, Piece *position[8][8], char figure) {
  Piece *newPiece = nullptr;
  switch (figure) {
  case 'Q':
    newPiece = new Queen(move.white);
    break;
  case 'R':
    newPiece = new Rook(move.white);
    break;
  case 'N':
    newPiece = new Knight(move.white);
    break;
  case 'B':
    newPiece = new Bishop(move.white);
    break;
  default:
    newPiece = new Queen(move.white);
    break;
  }
  delete position[move.target_row][move.target_column];
  position[move.target_row][move.target_column] = newPiece;
}

void reconstruct_move(Piece *board_now[8][8], Piece *board_prev[8][8],
                      char *move_buffer) {
  int src_r = -1, src_c = -1, dst_r = -1, dst_c = -1;
  int captured_r = -1, captured_c = -1;

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      bool wasFull = (board_prev[row][col] != nullptr);
      bool isFull = (board_now[row][col] != nullptr);

      if (wasFull && isFull &&
          board_prev[row][col]->symbol() == board_now[row][col]->symbol())
        continue;

      if (wasFull && !isFull) {
        char fp = board_prev[row][col]->symbol();
        if (fp == 'k' || fp == 'K') {
          src_r = row;
          src_c = col;
        } else {
          if (src_r == -1) {
            src_r = row;
            src_c = col;
          } else {
            captured_r = row;
            captured_c = col;
          }
        }
      } else if (!wasFull && isFull) {
        char fn = board_now[row][col]->symbol();
        if (fn == 'k' || fn == 'K') {
          dst_r = row;
          dst_c = col;
        } else if (dst_r == -1) {
          dst_r = row;
          dst_c = col;
        }
      }
    }
  }

  if (captured_r != -1 && dst_r != -1 && src_r != -1 &&
      board_now[dst_r][dst_c] != nullptr) {
    char movingPiece = board_now[dst_r][dst_c]->symbol();
    char prevSrc = board_prev[src_r][src_c]->symbol();
    if (prevSrc != movingPiece &&
        board_prev[captured_r][captured_c] != nullptr &&
        board_prev[captured_r][captured_c]->symbol() == movingPiece) {
      int tr = src_r, tc = src_c;
      src_r = captured_r;
      src_c = captured_c;
      captured_r = tr;
      captured_c = tc;
    }
  }

  if (src_r != -1 && dst_r != -1) {
    move_buffer[0] = (char)(src_c + 'A');
    move_buffer[1] = (char)(src_r + '1');
    move_buffer[2] = (char)(dst_c + 'A');
    move_buffer[3] = (char)(dst_r + '1');
    move_buffer[4] = '\0';
  }
}

void format_chess_notation(const Input &move, char *buffer) {
  if (move.figure == 'K' && abs(move.target_column - move.orig_column) == 2) {
    strcpy(buffer, move.target_column > move.orig_column ? "O-O" : "O-O-O");
    return;
  }
  char sep = move.capture ? 'x' : '-';
  if (move.figure == 'P') {
    snprintf(buffer, 8, "%c%d%c%c%d", move.orig_column + 'a', move.orig_row + 1,
             sep, move.target_column + 'a', move.target_row + 1);
  } else {
    snprintf(buffer, 8, "%c%c%d%c%c%d", move.figure, move.orig_column + 'a',
             move.orig_row + 1, sep, move.target_column + 'a',
             move.target_row + 1);
  }
}

int check_eval(Piece *board[8][8]) {
  int eval = 0;
  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++) {
      Piece *p = board[r][c];
      if (p == nullptr)
        continue;
      eval += isWhitePiece(p) ? p->value() : -p->value();
    }
  return eval;
}