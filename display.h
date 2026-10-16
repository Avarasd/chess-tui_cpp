/*
Tóth Avar Áron
DQMKMR
*/

#ifndef DISPLAY_H
#define DISPLAY_H
#include "chess.h"

enum State {
  STATE_MAIN_MENU,
  STATE_GAME_MENU,
  STATE_GAME_RUNNING,
  STATE_GAME_END,
  STATE_ANALYSIS_MENU,
  STATE_ANALYSIS_SETUP,
  STATE_ANALYSIS_RUNNING,
  STATE_ANALYSIS_END,
  STATE_EXIT
};

enum Coords {
  COORD_BOARD_X = 32,
  COORD_BOARD_Y = 5,
  COORD_INFO_X = 75,
  COORD_INFO_Y = 5,
  COORD_INPUT_X = 31,
  COORD_INPUT_Y = 17,
  COORD_MENU_X = 31,
  COORD_MENU_Y = 5
};

class Display {
public:
  static void init();
  static void clear();
  static void sleep(int seconds);
  static void flush();

  static void drawSquare(int x1, int y1, int x2, int y2);

  static void board(Piece *position[8][8], int x, int y);
  static void info(bool isWhiteTurn, int move, int eval);
  static void gameState(bool isValid, bool isCheck, bool isMate,
                        bool isStalemate);
  static void updateMoves(char *move, int moveCount);
  static void allAlternativeMoves(char (*moveArr)[5], int size);

  static void setEnPassantInfo(char col);

  static void getInput(char *input);
  static int getChar();
  static int getBranch();
  static char askPromotion();

  static State menu();
  static State gameModeMenu();
  static State analysisModeMenu();
  static State gameModeEnd(char *buffer);
  static bool analysisModeFileSet(char *filename);
  static int analysisInfo(int eval);
};

#endif // DISPLAY_H