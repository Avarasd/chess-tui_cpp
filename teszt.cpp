/*
Tóth Avar Áron
DQMKMR
*/

#include "chess.h"
#include "gtest_lite.h"

#include "storage.h"
#include <cstring>
#include <iostream>

#include "memtrace.h"
static void emptyBoard(Piece *board[8][8]) {
  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++)
      board[r][c] = nullptr;
}

int main() {
  std::cout << "=== Sakk tesztek ===" << std::endl;

  TEST(Basic, StartNotInCheck) {
    Piece *board[8][8];
    init_board(board);
    EXPECT_FALSE(is_king_in_check(board, true));
    EXPECT_FALSE(is_king_in_check(board, false));
    free_board(board);
  }
  END

  TEST(Basic, StartEvalZero) {
    Piece *board[8][8];
    init_board(board);
    EXPECT_EQ(0, check_eval(board));
    free_board(board);
  }
  END

  TEST(Pawn, E2E4Legal) {
    Piece *board[8][8];
    init_board(board);
    char mv[] = "E2E4";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Pawn, E2E5Illegal) {
    Piece *board[8][8];
    init_board(board);
    char mv[] = "E2E5";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Pawn, DiagonalToEmptyIllegal) {
    Piece *board[8][8];
    init_board(board);
    char mv[] = "E2D3";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Rule, BlackCannotMoveInWhiteTurn) {
    Piece *board[8][8];
    init_board(board);
    char mv[] = "E7E5";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Check, QueenDeliversCheck) {
    Piece *board[8][8];
    emptyBoard(board);
    board[7][5] = new King(false);
    board[6][5] = new Queen(true);
    board[0][4] = new King(true);
    EXPECT_TRUE(is_king_in_check(board, false));
    free_board(board);
  }
  END

  TEST(Check, KingIntoCheckIllegal) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[7][4] = new King(false);
    board[1][3] = new Rook(false);
    board[1][5] = new Rook(false);
    char mv[] = "E1E2";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(FileIO, SaveAndLoad) {
    Piece *board[8][8];
    init_board(board);
    BoardTree tree;
    tree.root = tree.createRoot();
    copy_board(board, tree.root->board);
    const char *fname = "chess_teszt_mentes.dat";
    tree.save(fname, tree.root);
    BoardTree tree2;
    bool ok = false;
    try {
      tree2.load(fname);
      ok = (tree2.root != nullptr);
      if (ok) {
        Piece *k = tree2.root->board[0][4];
        EXPECT_TRUE(k != nullptr);
        if (k)
          EXPECT_EQ('k', k->symbol());
      }
    } catch (const FileNotFoundException &) {
      ok = false;
    }
    EXPECT_TRUE(ok);
    free_board(board);
  }
  END

  TEST(FileIO, MissingFileException) {
    BoardTree tree;
    bool caught = false;
    try {
      tree.load("/tmp/nem_letezik_fajl_99999.dat");
    } catch (const FileNotFoundException &) {
      caught = true;
    }
    EXPECT_TRUE(caught);
  }
  END

  TEST(Piece, CloneCorrect) {
    Piece *p = new Queen(true);
    Piece *q = p->clone();
    EXPECT_TRUE(q != nullptr);
    EXPECT_EQ(p->symbol(), q->symbol());
    EXPECT_EQ(p->value(), q->value());
    EXPECT_TRUE(p != q);
    delete p;
    delete q;
  }
  END

  TEST(Rule, CastlingLegal) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[0][7] = new Rook(true);
    char mv[] = "E1G1";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Rule, EnPassantLegal) {
    Piece *board[8][8];
    emptyBoard(board);
    board[4][4] = new Pawn(true);
    board[4][3] = new Pawn(false);
    char mv[] = "E5D6";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, false, b, 3, nullptr));
    free_board(board);
  }
  END

  TEST(Rule, Stalemate) {
    Piece *board[8][8];
    emptyBoard(board);
    board[7][0] = new King(false);
    board[6][2] = new Queen(true);
    board[5][0] = new King(true);
    EXPECT_FALSE(any_valid_moves(board, false, -1));
    free_board(board);
  }
  END

  TEST(PieceFromSymbol, WhitePieceLegal) {
    Piece *p = pieceFromSymbol('r');
    EXPECT_TRUE(p != nullptr);
    EXPECT_EQ('r', p->symbol());
    delete p;
  }
  END

  TEST(PieceFromSymbol, InvalidCharacter) {
    Piece *p = pieceFromSymbol('X');
    EXPECT_TRUE(p == nullptr);
  }
  END

  TEST(Pawn, E7E5Legal) {
    Piece *board[8][8];
    emptyBoard(board);
    board[6][4] = new Pawn(false);
    board[7][4] = new King(false);
    board[0][4] = new King(true);
    char mv[] = "E7E5";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(
        is_move_pattern_valid(inp, board, false, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Pawn, BlackEnPassant) {
    Piece *board[8][8];
    emptyBoard(board);
    board[7][4] = new King(false);
    board[0][4] = new King(true);
    board[3][3] = new Pawn(false);
    board[3][4] = new Pawn(true);
    char mv[] = "D4E3";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, false, false, b, 4, nullptr));
    free_board(board);
  }
  END

  TEST(Pawn, E2E4Blocked) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[7][4] = new King(false);
    board[1][4] = new Pawn(true);
    board[2][4] = new Pawn(false);
    char mv[] = "E2E4";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Rook, CanMoveInFreeRank) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[7][4] = new King(false);
    board[4][0] = new Rook(true);
    char mv[] = "A5H5";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Rook, PathBlocked) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[7][4] = new King(false);
    board[4][0] = new Rook(true);
    board[4][3] = new Pawn(true);
    char mv[] = "A5H5";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Bishop, CanMoveDiagonally) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[7][4] = new King(false);
    board[0][2] = new Bishop(true);
    char mv[] = "C1F4";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Queen, CanMoveDiagonally) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[7][4] = new King(false);
    board[3][3] = new Queen(true);
    char mv[] = "D4G7";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(CurrMove, InvalidInput) {
    Piece *board[8][8];
    init_board(board);
    char mv[] = "Z9Z9";
    Input inp = curr_move(mv, board);
    EXPECT_EQ(0, inp.orig_row);
    EXPECT_EQ(0, inp.orig_column);
    free_board(board);
  }
  END

  TEST(Castling, ShortCastlingFailsIfKingMoved) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[0][7] = new Rook(true);
    char mv[] = "E1G1";
    Input inp = curr_move(mv, board);
    BoolTable b;
    b.white_king_moved = true;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Castling, LongCastlingLegal) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[0][0] = new Rook(true);
    char mv[] = "E1C1";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(Castling, CastlingPathBlocked) {
    Piece *board[8][8];
    emptyBoard(board);
    board[0][4] = new King(true);
    board[0][7] = new Rook(true);
    board[0][5] = new Bishop(true);
    char mv[] = "E1G1";
    Input inp = curr_move(mv, board);
    BoolTable b;
    EXPECT_FALSE(
        is_move_pattern_valid(inp, board, true, false, b, -1, nullptr));
    free_board(board);
  }
  END

  TEST(MoveModify, BoardChanges) {
    Piece *board[8][8];
    init_board(board);
    char mv[] = "E2E4";
    Input inp = curr_move(mv, board);
    BoolTable b;
    int nextEp = -1;
    EXPECT_TRUE(is_move_pattern_valid(inp, board, true, true, b, -1, &nextEp));
    EXPECT_TRUE(board[3][4] != nullptr);
    EXPECT_TRUE(board[1][4] == nullptr);
    EXPECT_EQ(4, nextEp);
    free_board(board);
  }
  END

  TEST(BoolChecker, KingMovedFlag) {
    Piece *board[8][8];
    init_board(board);
    delete board[0][4];
    board[0][4] = nullptr;
    BoolTable b;
    bool_checker(board, &b);
    EXPECT_TRUE(b.white_king_moved);
    free_board(board);
  }
  END

  TEST(BoolChecker, RookMovedFlag) {
    Piece *board[8][8];
    init_board(board);
    delete board[0][0];
    board[0][0] = nullptr;
    BoolTable b;
    bool_checker(board, &b);
    EXPECT_TRUE(b.white_rook_A_moved);
    free_board(board);
  }
  END

  TEST(PawnPromotion, WhiteInLastRank) {
    Input move(7, 4, 7, 4, 'P', false, true);
    EXPECT_TRUE(pawn_promotion(move));
  }
  END

  TEST(PawnPromotion, BlackInFirstRank) {
    Input move(0, 4, 0, 4, 'P', false, false);
    EXPECT_TRUE(pawn_promotion(move));
  }
  END

  TEST(Promotion, PromoteToQueen) {
    Piece *board[8][8];
    emptyBoard(board);
    board[7][4] = new Pawn(true);
    board[0][4] = new King(true);
    board[6][4] = new King(false);
    Input move(1, 4, 7, 4, 'P', false, true);
    do_promotion(move, board, 'Q');
    EXPECT_TRUE(board[7][4] != nullptr);
    EXPECT_EQ('q', board[7][4]->symbol());
    free_board(board);
  }
  END

  TEST(Promotion, UnknownPieceDefaultsToQueen) {
    Piece *board[8][8];
    emptyBoard(board);
    board[7][4] = new Pawn(true);
    board[0][4] = new King(true);
    board[6][4] = new King(false);
    Input move(1, 4, 7, 4, 'P', false, true);
    do_promotion(move, board, 'X');
    EXPECT_EQ('q', board[7][4]->symbol());
    free_board(board);
  }
  END

  TEST(ReconstructMove, SimpleMove) {
    Piece *prev[8][8];
    Piece *now[8][8];
    init_board(prev);
    init_board(now);
    now[3][4] = now[1][4];
    now[1][4] = nullptr;
    char buf[5] = {0};
    reconstruct_move(now, prev, buf);
    EXPECT_EQ('E', buf[0]);
    EXPECT_EQ('2', buf[1]);
    EXPECT_EQ('E', buf[2]);
    EXPECT_EQ('4', buf[3]);
    free_board(prev);
    free_board(now);
  }
  END

  TEST(FormatNotation, SimplePawnMove) {
    Input move(1, 4, 3, 4, 'P', false, true);
    char buf[16] = {0};
    format_chess_notation(move, buf);
    EXPECT_EQ('e', buf[0]);
    EXPECT_EQ('2', buf[1]);
    EXPECT_EQ('-', buf[2]);
    EXPECT_EQ('e', buf[3]);
    EXPECT_EQ('4', buf[4]);
  }
  END

  TEST(FormatNotation, ShortCastling) {
    Input move(0, 4, 0, 6, 'K', false, true);
    char buf[16] = {0};
    format_chess_notation(move, buf);
    EXPECT_EQ('O', buf[0]);
    EXPECT_EQ('-', buf[1]);
    EXPECT_EQ('O', buf[2]);
    EXPECT_EQ('\0', buf[3]);
  }
  END

  TEST(Storage, AddBoardDirectly) {
    BoardTree tree;
    tree.root = tree.createRoot();
    EXPECT_TRUE(tree.root != nullptr);
    Board *child = tree.addBoard(tree.root);
    EXPECT_TRUE(child != nullptr);
    EXPECT_EQ(1, tree.root->numNext);
    EXPECT_EQ(tree.root->next[0], child);
    EXPECT_EQ(tree.root, child->prev);
  }
  END

  TEST(Storage, MultiNodeSaveAndLoad) {
    BoardTree tree;
    tree.root = tree.createRoot();
    Piece *board[8][8];
    init_board(board);
    copy_board(board, tree.root->board);
    Board *child = tree.addBoard(tree.root);
    copy_board(board, child->board);
    const char *fname = "chess_multinodetes.dat";
    tree.save(fname, tree.root);
    BoardTree tree2;
    bool ok = false;
    try {
      tree2.load(fname);
      ok = (tree2.root != nullptr && tree2.root->numNext == 1);
    } catch (...) {
    }
    EXPECT_TRUE(ok);
    free_board(board);
  }
  END

  TEST(Storage, LastMoveCountThreeMoves) {
    BoardTree tree;
    tree.root = tree.createRoot();
    Board *c1 = tree.addBoard(tree.root);
    Board *c2 = tree.addBoard(c1);
    tree.addBoard(c2);
    EXPECT_EQ(3, tree.lastMoveCount(tree.root));
  }
  END

  TEST(Storage, FindLastThreeMoves) {
    BoardTree tree;
    tree.root = tree.createRoot();
    Board *c1 = tree.addBoard(tree.root);
    Board *c2 = tree.addBoard(c1);
    Board *c3 = tree.addBoard(c2);
    Board *last = tree.findLast(tree.root);
    EXPECT_EQ(c3, last);
  }
  END

  TEST(Storage, SearchLinearValid) {
    BoardTree tree;
    tree.root = tree.createRoot();
    Board *c1 = tree.addBoard(tree.root);
    Board *found = tree.searchLinear(tree.root, 1);
    EXPECT_EQ(c1, found);
  }
  END

  TEST(Storage, SearchLinearInvalid) {
    BoardTree tree;
    tree.root = tree.createRoot();
    tree.addBoard(tree.root);
    Board *found = tree.searchLinear(tree.root, 5);
    EXPECT_TRUE(found == nullptr);
  }
  END return 0;
}
