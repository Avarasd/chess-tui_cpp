/*
Tóth Avar Áron
DQMKMR
*/

#include "storage.h"
#include "chess.h"
#include <cstring>
#include <fstream>

#include "memtrace.h"

#pragma pack(1)
struct BoardElement {
  char board[8][8];
  int id;
  int previd;
  int en_passant_col;
};
#pragma pack()

Board::Board(Board *parent)
    : prev(parent), numNext(0), next(nullptr), id(0),
      previd(parent != nullptr ? parent->id : -1), selectedBranch(0),
      en_passant_col(-1) {
  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++)
      board[r][c] = nullptr;
}

Board::~Board() { free_board(board); }

BoardTree::BoardTree() : root(nullptr), m_maxid(0) {}

BoardTree::~BoardTree() {
  if (root != nullptr) {
    freeAll(root);
    root = nullptr;
  }
}

void BoardTree::updateMaxid(int id) {
  if (id > m_maxid)
    m_maxid = id;
}

Board *BoardTree::searchById(int id, Board *curr) const {
  if (curr == nullptr)
    return nullptr;
  if (curr->id == id)
    return curr;
  for (int i = 0; i < curr->numNext; i++) {
    Board *found = searchById(id, curr->next[i]);
    if (found != nullptr)
      return found;
  }
  return nullptr;
}

Board *BoardTree::addBoard(Board *parent) {
  Board **newArr = new Board *[parent->numNext + 1];
  for (int i = 0; i < parent->numNext; i++)
    newArr[i] = parent->next[i];
  delete[] parent->next;
  parent->next = newArr;

  Board *nextBoard = new Board(parent);
  nextBoard->id = m_maxid++;
  parent->next[parent->numNext] = nextBoard;
  parent->numNext++;
  return nextBoard;
}

void BoardTree::freeAll(Board *node) {
  if (node == nullptr)
    return;
  if (node->next != nullptr) {
    for (int i = 0; i < node->numNext; i++)
      freeAll(node->next[i]);
    delete[] node->next;
    node->next = nullptr;
  }
  delete node;
}

void BoardTree::saveboard(std::ofstream &out, Board *curr) const {
  BoardElement element;
  element.id = curr->id;
  element.previd = curr->previd;
  element.en_passant_col = curr->en_passant_col;

  for (int r = 0; r < 8; r++)
    for (int c = 0; c < 8; c++)
      element.board[r][c] =
          (curr->board[r][c] != nullptr) ? curr->board[r][c]->symbol() : '.';

  out.write(reinterpret_cast<const char *>(&element), sizeof(BoardElement));
  for (int i = 0; i < curr->numNext; i++)
    saveboard(out, curr->next[i]);
}

void BoardTree::save(const char *filename, Board *head) const {
  std::ofstream out(filename, std::ios::binary);
  if (!out)
    return;
  saveboard(out, head);
}

Board *BoardTree::createFromElement(BoardElement *element) {
  Board *curr = nullptr;

  if (element->previd == -1) {
    curr = new Board(nullptr);
    curr->id = element->id;
    root = curr;
  } else {
    Board *parent = searchById(element->previd, root);
    if (parent == nullptr)
      return nullptr;
    curr = addBoard(parent);
  }

  if (curr != nullptr) {
    for (int r = 0; r < 8; r++)
      for (int c = 0; c < 8; c++)
        curr->board[r][c] = (element->board[r][c] != '.')
                                ? pieceFromSymbol(element->board[r][c])
                                : nullptr;
    curr->id = element->id;
    curr->previd = element->previd;
    curr->en_passant_col = element->en_passant_col;
    updateMaxid(curr->id);
  }
  return curr;
}

void BoardTree::load(const char *filename) {
  std::ifstream in(filename, std::ios::binary);
  if (!in)
    throw FileNotFoundException(filename);
  BoardElement element;
  while (in.read(reinterpret_cast<char *>(&element), sizeof(element)))
    createFromElement(&element);
}

Board *BoardTree::findLast(Board *curr) const {
  Board *last = curr;
  while (last->numNext > 0)
    last = last->next[0];
  return last;
}

Board *BoardTree::searchLinear(Board *head, int moveCount) const {
  if (head == nullptr)
    return nullptr;
  Board *found = head;
  for (int i = 0; i < moveCount; i++) {
    if (found->numNext <= 0 || found->next == nullptr)
      return nullptr;
    found = found->next[0];
  }
  return found;
}

int BoardTree::lastMoveCount(Board *head) const {
  Board *curr = head;
  int count = 0;
  while (curr->numNext > 0) {
    curr = curr->next[0];
    count++;
  }
  return count;
}

Board *BoardTree::createRoot() {
  freeAll(root);
  root = new Board(nullptr);
  root->id = 0;
  m_maxid = 1;
  return root;
}