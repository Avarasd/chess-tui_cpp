/*
Tóth Avar Áron
DQMKMR
*/

#include "chess.h"
#include "display.h"
#include "storage.h"
#include <cstring>
#include <iostream>

#include "memtrace.h"

namespace {

void booleans_init(Booleans *b) {
  b->check = false;
  b->mate = false;
  b->stalemate = false;
  b->isWhiteTurn = true;
  b->white_king_moved = false;
  b->black_king_moved = false;
  b->white_rook_A_moved = false;
  b->white_rook_H_moved = false;
  b->black_rook_A_moved = false;
  b->black_rook_H_moved = false;
  b->playing = true;
}

void all_alternative_moves(Board *curr) {
  char(*allMoves)[5] = new char[curr->numNext > 0 ? curr->numNext : 1][5];
  for (int i = 0; i < curr->numNext; i++)
    reconstruct_move(curr->next[i]->board, curr->board, allMoves[i]);
  Display::allAlternativeMoves(allMoves, curr->numNext);
  delete[] allMoves;
}

void bool_checker_reverse(Board *board, Booleans *b) {
  Board *curr = board;
  while (curr->prev != nullptr) {
    bool_checker(curr->board, b);
    curr = curr->prev;
  }
}

void load_all_moves(BoardTree &tree) {
  Board *curr = tree.root;
  int move_count = 1;
  while (curr->numNext > 0) {
    char move[5];
    reconstruct_move(curr->next[0]->board, curr->board, move);
    Input formatted_input = curr_move(move, curr->board);
    char formatted_move[8];
    format_chess_notation(formatted_input, formatted_move);
    Display::updateMoves(formatted_move, move_count);
    curr = curr->next[0];
    move_count++;
  }
  Display::flush();
}

State game_mode_display(BoardTree &tree, Booleans game_booleans) {
  if (tree.root != nullptr)
    tree.freeAll(tree.root);
  tree.root = tree.createRoot();

  init_board(tree.root->board);

  Board *curr_node = tree.root;
  booleans_init(&game_booleans);
  Display::clear();
  int move_count = 1;

  while (!game_booleans.mate && !game_booleans.stalemate &&
         game_booleans.playing) {
    int eval = check_eval(curr_node->board);
    Display::board(curr_node->board, COORD_BOARD_X, COORD_BOARD_Y);
    Display::info(game_booleans.isWhiteTurn, move_count, eval);

    char move[5];
    Display::getInput(move);
    if (std::cin.eof())
      return STATE_EXIT;
    Input formatted_input = curr_move(move, curr_node->board);
    int next_en_passant_col = -1;

    if (is_move_pattern_valid(formatted_input, curr_node->board,
                              game_booleans.isWhiteTurn, false, game_booleans,
                              curr_node->en_passant_col, nullptr)) {

      Board *next_node = tree.addBoard(curr_node);
      if (next_node != nullptr) {
        copy_board(curr_node->board, next_node->board);
        is_move_pattern_valid(formatted_input, next_node->board,
                              game_booleans.isWhiteTurn, true, game_booleans,
                              curr_node->en_passant_col, &next_en_passant_col);
        next_node->en_passant_col = next_en_passant_col;

        if (pawn_promotion(formatted_input)) {
          char choice = Display::askPromotion();
          do_promotion(formatted_input, next_node->board, choice);
        }

        bool_checker(next_node->board, &game_booleans);
        char formatted_move[8];
        format_chess_notation(formatted_input, formatted_move);
        Display::updateMoves(formatted_move, move_count);

        curr_node = next_node;
      }

      game_booleans.isWhiteTurn = !game_booleans.isWhiteTurn;
      move_count++;
      game_booleans.check =
          is_king_in_check(curr_node->board, game_booleans.isWhiteTurn);

      if (!any_valid_moves(curr_node->board, game_booleans.isWhiteTurn,
                           curr_node->en_passant_col)) {
        game_booleans.mate = game_booleans.check;
        game_booleans.stalemate = !game_booleans.check;
        Display::gameState(true, game_booleans.check, game_booleans.mate,
                           game_booleans.stalemate);
        Display::board(curr_node->board, COORD_BOARD_X, COORD_BOARD_Y);
        Display::sleep(5);
      } else {
        Display::gameState(true, game_booleans.check, game_booleans.mate,
                           game_booleans.stalemate);
      }
    } else {
      Display::gameState(false, game_booleans.check, game_booleans.mate,
                         game_booleans.stalemate);
    }
  }
  return STATE_GAME_END;
}

State analysis_mode_display(BoardTree &tree) {
  if (tree.root == nullptr)
    return STATE_ANALYSIS_MENU;

  Board *curr_board = tree.root;
  Display::clear();
  int move_count = 0;
  Booleans analysis_booleans;
  booleans_init(&analysis_booleans);
  load_all_moves(tree);

  while (true) {
    Display::board(curr_board->board, COORD_BOARD_X, COORD_BOARD_Y);
    all_alternative_moves(curr_board);
    int eval = check_eval(curr_board->board);
    int input = Display::analysisInfo(eval);

    switch (input) {
    case 'l': {
      if (curr_board != tree.root) {
        curr_board = curr_board->prev;
        move_count--;
      }
      break;
    }
    case 'r': {
      if (curr_board->numNext != 0) {
        if (curr_board->selectedBranch >= curr_board->numNext)
          curr_board->selectedBranch = 0;
        curr_board = curr_board->next[curr_board->selectedBranch];
        move_count++;
      }
      break;
    }
    case '1': {
      curr_board = tree.root;
      move_count = 0;
      break;
    }
    case '2': {
      curr_board = tree.findLast(curr_board);
      move_count = tree.lastMoveCount(tree.root);
      break;
    }
    case '3': {
      move_count = Display::getChar();
      if (move_count < 0)
        move_count = 1;
      int maxMove = tree.lastMoveCount(tree.root);
      if (move_count > maxMove)
        move_count = maxMove;
      Board *result = tree.searchLinear(tree.root, move_count);
      if (result != nullptr)
        curr_board = result;
      break;
    }
    case '4': {
      char move[5];
      Display::getInput(move);
      if (std::cin.eof())
        return STATE_ANALYSIS_END;
      Input formatted_input = curr_move(move, curr_board->board);
      bool_checker_reverse(curr_board, &analysis_booleans);
      bool whiteTurn = !(move_count % 2);
      if (is_move_pattern_valid(formatted_input, curr_board->board, whiteTurn,
                                false, analysis_booleans,
                                curr_board->en_passant_col, nullptr)) {
        Board *new_board = tree.addBoard(curr_board);
        if (new_board != nullptr) {
          copy_board(curr_board->board, new_board->board);
          is_move_pattern_valid(formatted_input, new_board->board, whiteTurn,
                                true, analysis_booleans,
                                curr_board->en_passant_col,
                                &new_board->en_passant_col);
          if (pawn_promotion(formatted_input)) {
            char choice = Display::askPromotion();
            do_promotion(formatted_input, new_board->board, choice);
          }
          analysis_booleans.check =
              is_king_in_check(new_board->board, whiteTurn);
          curr_board = new_board;
          move_count++;
        } else {
          Display::gameState(false, false, false, false);
        }
      }
      break;
    }
    case '5': {
      if (curr_board->numNext > 0) {
        int choice = Display::getBranch();
        if (choice > 0 && choice <= curr_board->numNext) {
          curr_board->selectedBranch = choice - 1;
          curr_board = curr_board->next[choice - 1];
          move_count++;
        }
      }
      break;
    }
    case 'x':
      return STATE_ANALYSIS_END;
    }
  }
}

} // namespace

int main() {
  Booleans game_booleans;
  Display::init();

  State currentState = STATE_MAIN_MENU;
  char fileName[200] = {0};
  bool exited = false;
  BoardTree tree;

  while (!exited) {
    switch (currentState) {
    case STATE_MAIN_MENU:
      currentState = Display::menu();
      break;
    case STATE_GAME_MENU:
      currentState = Display::gameModeMenu();
      break;
    case STATE_GAME_RUNNING:
      currentState = game_mode_display(tree, game_booleans);
      break;
    case STATE_ANALYSIS_MENU:
      currentState = Display::analysisModeMenu();
      break;
    case STATE_ANALYSIS_SETUP:
      try {
        if (Display::analysisModeFileSet(fileName)) {
          tree.load(fileName);
          currentState = STATE_ANALYSIS_RUNNING;
        } else {
          currentState = STATE_ANALYSIS_MENU;
        }
      } catch (const FileNotFoundException &e) {
        currentState = STATE_ANALYSIS_MENU;
      }
      break;
    case STATE_ANALYSIS_RUNNING:
      currentState = analysis_mode_display(tree);
      break;
    case STATE_GAME_END:
      currentState = Display::gameModeEnd(fileName);
      tree.save(fileName, tree.root);
      tree.freeAll(tree.root);
      tree.root = nullptr;
      break;
    case STATE_ANALYSIS_END:
      currentState = STATE_MAIN_MENU;
      tree.save(fileName, tree.root);
      tree.freeAll(tree.root);
      tree.root = nullptr;
      break;
    case STATE_EXIT:
      exited = true;
      break;
    }
  }

  if (tree.root != nullptr) {
    tree.freeAll(tree.root);
    tree.root = nullptr;
  }
  return 0;
}
