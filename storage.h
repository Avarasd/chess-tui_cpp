/*
Tóth Avar Áron
DQMKMR
*/

#ifndef STORAGE_H
#define STORAGE_H
#include <exception>
#include <iosfwd>
#include <string>

class Piece;

class FileNotFoundException : public std::exception {
  std::string m_msg;

public:
  explicit FileNotFoundException(const std::string &filename)
      : m_msg("Nem sikerult megnyitni: " + filename) {}
  const char *what() const throw() override { return m_msg.c_str(); }
};

struct Board {
  Piece *board[8][8];
  Board *prev;
  int numNext;
  Board **next;
  int id;
  int previd;
  int selectedBranch;
  int en_passant_col;

  explicit Board(Board *parent = nullptr);
  ~Board();
};

class BoardTree {
public:
  Board *root;

  BoardTree();
  ~BoardTree();

  Board *addBoard(Board *parent);
  void freeAll(Board *node);

  void save(const char *filename, Board *head) const;
  void load(const char *filename);

  Board *findLast(Board *curr) const;
  Board *searchLinear(Board *head, int moveCount) const;
  int lastMoveCount(Board *head) const;
  Board *createRoot();

private:
  int m_maxid;

  void updateMaxid(int id);
  Board *searchById(int id, Board *curr) const;
  void saveboard(std::ofstream &out, Board *curr) const;
  Board *createFromElement(struct BoardElement *element);
};

#endif // STORAGE_H