/*
Tóth Avar Áron
DQMKMR
*/

#include "display.h"
#include "chess.h"
#include "econio.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "memtrace.h"

void Display::init() {
#ifdef _WIN32
  SetConsoleOutputCP(65001);
#else
  printf("\033[8;30;115t");
  fflush(stdout);
#endif
  econio_rawmode();
  econio_textbackground(COL_BLACK);
  econio_textcolor(COL_WHITE);
  econio_clrscr();
}

void Display::clear() {
  econio_textbackground(COL_BLACK);
  econio_clrscr();
}

void Display::sleep(int seconds) { econio_sleep(seconds); }

void Display::flush() { econio_flush(); }

void Display::drawSquare(int x1, int y1, int x2, int y2) {
  econio_gotoxy(x1, y1);
  std::cout << '+';
  econio_gotoxy(x2, y1);
  std::cout << '+';
  econio_gotoxy(x1, y2);
  std::cout << '+';
  econio_gotoxy(x2, y2);
  std::cout << '+';
  for (int i = x1 + 1; i < x2; i++) {
    econio_gotoxy(i, y1);
    std::cout << '-';
    econio_gotoxy(i, y2);
    std::cout << '-';
  }
  for (int i = y1 + 1; i < y2; i++) {
    econio_gotoxy(x1, i);
    std::cout << '|';
    econio_gotoxy(x2, i);
    std::cout << '|';
  }
}

int Display::getChar() {
  int input = -1;
  econio_gotoxy(COORD_INPUT_X, COORD_INPUT_Y);
  std::cout << "Adj meg egy lépésszámot:";
  drawSquare(30, 16, 62, 18);
  econio_gotoxy(55, COORD_INPUT_Y);
  econio_normalmode();
  char buffer[20];
  if (std::cin.getline(buffer, sizeof(buffer)))
    try {
      input = std::stoi(buffer);
    } catch (...) {
      input = -1;
    }
  econio_rawmode();
  econio_gotoxy(55, COORD_INPUT_Y);
  for (int i = 0; i < 5; i++)
    std::cout << " ";
  return input;
}

int Display::getBranch() {
  int input = -1;
  econio_gotoxy(COORD_INPUT_X, COORD_INPUT_Y);
  std::cout << "Válassz sorszámot:";
  drawSquare(30, 16, 62, 18);
  econio_gotoxy(55, COORD_INPUT_Y);
  econio_normalmode();
  char buffer[20];
  if (std::cin.getline(buffer, sizeof(buffer)))
    try {
      input = std::stoi(buffer);
    } catch (...) {
      input = -1;
    }
  econio_rawmode();
  econio_gotoxy(55, COORD_INPUT_Y);
  for (int i = 0; i < 5; i++)
    std::cout << " ";
  return input;
}

void Display::board(Piece *position[8][8], int x, int y) {
  econio_textcolor(COL_CYAN);
  econio_gotoxy(x + 3, y);
  std::cout << " A  B  C  D  E  F  G  H";
  for (int row = 7; row >= 0; row--) {
    econio_gotoxy(x, y + 1 + (7 - row));
    std::cout << row + 1;
    for (int column = 0; column < 8; column++) {
      econio_gotoxy(x + 3 + (column * 3), y + 1 + (7 - row));
      Piece *p = position[row][column];
      bool black_square = (row + column) % 2 == 0;
      econio_textbackground(black_square ? COL_BLACK : COL_DARKGRAY);
      if (p == nullptr) {
        std::cout << "   ";
      } else {
        char sym = p->symbol();
        bool pieceIsWhite = (sym >= 'a' && sym <= 'z');
        econio_textcolor(pieceIsWhite ? COL_YELLOW : COL_LIGHTGRAY);
        std::cout << " " << sym << " ";
      }
    }
    econio_gotoxy(x + 3 + (8 * 3) + 1, y + 1 + (7 - row));
    econio_textbackground(COL_BLACK);
    econio_textcolor(COL_CYAN);
    std::cout << row + 1;
  }
  econio_textcolor(COL_CYAN);
  econio_gotoxy(x + 3, y + 10);
  std::cout << " A  B  C  D  E  F  G  H";
  econio_textcolor(COL_WHITE);
  econio_textbackground(COL_BLACK);
}

void Display::info(bool isWhiteTurn, int move, int eval) {
  econio_textcolor(COL_WHITE);
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y);
  std::cout << "                        ";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y);
  std::cout << move << ". LÉPÉS, " << (isWhiteTurn ? "VILÁGOS" : "SÖTÉT")
            << " JÖN";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 1);
  std::cout << "Értékelés: " << std::setw(2) << eval;
}

void Display::gameState(bool isValid, bool isCheck, bool isMate,
                        bool isStalemate) {
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 2);
  for (int i = 0; i < 30; i++)
    std::cout << " ";
  econio_textcolor(COL_RED);
  econio_textbackground(COL_BLUE);
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 2);
  if (isValid) {
    if (isMate)
      std::cout << "MATT! A játéknak vége.";
    else if (isStalemate)
      std::cout << "PATT! A játéknak vége.";
    else if (isCheck)
      std::cout << "SAKK!";
    else
      std::cout << "Szabályos lépés!";
  } else {
    std::cout << "Szabálytalan lépés!";
  }
  econio_textcolor(COL_RESET);
  econio_textbackground(COL_BLACK);
}

void Display::getInput(char *input) {
  econio_gotoxy(COORD_INPUT_X, COORD_INPUT_Y);
  std::cout << "Adj meg egy lépést:";
  drawSquare(30, 16, 62, 18);
  econio_gotoxy(55, COORD_INPUT_Y);
  for (int i = 0; i < 5; i++)
    std::cout << " ";
  econio_gotoxy(55, COORD_INPUT_Y);
  econio_normalmode();
  char buffer[100];
  if (std::cin.getline(buffer, sizeof(buffer))) {
    buffer[strcspn(buffer, "\n")] = 0;
    strncpy(input, buffer, 4);
    input[4] = '\0';
  }
  econio_rawmode();
}

void Display::updateMoves(char *move, int moveCount) {
  int current_row = 5 + ((moveCount - 1) / 2);
  econio_gotoxy(6, 3);
  std::cout << "Lépések:";
  if (moveCount % 2 != 0) {
    econio_gotoxy(1, current_row);
    std::cout << "                             ";
    econio_gotoxy(2, current_row);
    std::cout << moveCount / 2 + 1 << ".";
    econio_gotoxy(6, current_row);
    std::cout << std::left << std::setw(10) << move << std::right;
    econio_gotoxy(1, current_row + 1);
    std::cout << "                             ";
  } else {
    econio_gotoxy(18, current_row);
    std::cout << std::left << std::setw(10) << move << std::right;
  }
  drawSquare(1, 4, 28, current_row + 1);
}

State Display::gameModeEnd(char *buffer) {
  char input[50];
  econio_clrscr();
  econio_gotoxy(34, 5);
  std::cout << "Add meg a parti nevét (max 20 karakter)";
  drawSquare(29, 4, 90, 9);
  econio_gotoxy(34, 7);
  econio_normalmode();
  if (std::cin.getline(input, 50))
    input[strcspn(input, "\n")] = 0;
#ifdef _WIN32
  _mkdir("Games");
  snprintf(buffer, 200, "Games\\%s.dat", input);
#else
  mkdir("Games", 0755);
  snprintf(buffer, 200, "Games/%s.dat", input);
#endif
  econio_rawmode();
  econio_gotoxy(34, 8);
  std::cout << "Sikeres mentés! Nyomj egy gombot a folytatáshoz";
  while (true)
    if (econio_kbhit())
      return STATE_MAIN_MENU;
}

State Display::gameModeMenu() {
  econio_clrscr();
  econio_gotoxy(34, 5);
  std::cout << "<<Üdvözöllek a játék módban>>";
  econio_gotoxy(31, 7);
  std::cout << "Nyomj meg egy gombot a kezdéshez!";
  econio_gotoxy(31, 9);
  std::cout << "Vagy nyomd meg a B-t visszalépéshez!";
  drawSquare(29, 4, 67, 11);
  econio_rawmode();
  while (true) {
    if (econio_kbhit()) {
      char inp = econio_getch();
      if (inp == 'b' || inp == 'B')
        return STATE_MAIN_MENU;
      else
        return STATE_GAME_RUNNING;
    }
  }
}

State Display::analysisModeMenu() {
  econio_clrscr();
  econio_gotoxy(34, 5);
  std::cout << "<<Üdvözöllek az elemző módban>>";
  econio_gotoxy(31, 7);
  std::cout << "Nyomj meg egy gombot a kezdéshez!";
  econio_gotoxy(31, 9);
  std::cout << "Vagy nyomd meg a B-t visszalépéshez!";
  drawSquare(29, 4, 67, 11);
  econio_rawmode();
  while (true) {
    if (econio_kbhit()) {
      char inp = econio_getch();
      if (inp == 'b' || inp == 'B')
        return STATE_MAIN_MENU;
      else
        return STATE_ANALYSIS_SETUP;
    }
  }
}

static bool selectFileFromList(char fileList[][40], int totalFiles,
                               char *filename) {
  int page = 0;
  int totalPages = (totalFiles + 9) / 10;
  while (true) {
    if (page < 0)
      page = 0;
    if (page >= totalPages)
      page = totalPages - 1;
    if (totalFiles == 0)
      return false;
    econio_clrscr();
    econio_gotoxy(31, 1);
    std::cout << "<Válassz egy fájlt> Oldal: " << page + 1 << "/" << totalPages;
    int start = page * 10;
    int end = start + 10;
    if (end > totalFiles)
      end = totalFiles;
    int itemsOnPage = end - start;
    for (int i = start; i < end; i++) {
      econio_gotoxy(31, 3 + (i % 10) * 2);
      int dNum = (i % 10) + 1;
      if (dNum == 10)
        dNum = 0;
      std::cout << dNum << ". " << fileList[i];
    }
    Display::drawSquare(29, 0, 67, 5 + itemsOnPage * 2);
    econio_gotoxy(75, 1);
    std::cout << "Az alábbi opciók közül válassz:";
    econio_gotoxy(75, 3);
    std::cout << "< Előző lapra lépés";
    econio_gotoxy(75, 5);
    std::cout << "> Következő lapra lépés";
    econio_gotoxy(75, 7);
    std::cout << "0-9: Fájl kiválasztása";
    econio_gotoxy(75, 9);
    std::cout << "X: kilépés";
    econio_gotoxy(32, 7 + itemsOnPage * 2);
    while (!econio_kbhit())
      ;
    int inp = econio_getch();
    switch (inp) {
    case KEY_LEFT:
      page--;
      break;
    case KEY_RIGHT:
      page++;
      break;
    case KEY_ESCAPE:
    case 'x':
    case 'b':
      return false;
    default: {
      if (inp >= '0' && inp <= '9') {
        int sel = inp - '0';
        if (sel == 0)
          sel = 10;
        if (sel <= itemsOnPage) {
#ifdef _WIN32
          snprintf(filename, 200, "Games\\%s", fileList[start + sel - 1]);
#else
          snprintf(filename, 200, "Games/%s", fileList[start + sel - 1]);
#endif
          return true;
        }
      } else {
        econio_gotoxy(32, 9 + itemsOnPage * 2);
        std::cout << "Érvénytelen sorszám";
        econio_sleep(1);
      }
    } break;
    }
  }
}

bool Display::analysisModeFileSet(char *filename) {
  econio_clrscr();
  char fileList[100][40];
  int totalFiles = 0;

#ifdef _WIN32
  WIN32_FIND_DATA findData;
  HANDLE hFind = FindFirstFile("Games\\*.dat", &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    econio_gotoxy(31, 3);
    std::cout << "Nincs betölthető fájl";
    econio_gotoxy(31, 5);
    std::cout << "Nyomj egy gombot a visszalépéshez";
    Display::drawSquare(29, 2, 67, 7);
    while (true)
      if (econio_kbhit())
        return false;
  }
  do {
    if (totalFiles < 100) {
      strcpy(fileList[totalFiles], findData.cFileName);
      totalFiles++;
    }
  } while (FindNextFile(hFind, &findData) != 0);
  FindClose(hFind);

#else
  DIR *dir = opendir("Games");
  if (dir == nullptr) {
    econio_gotoxy(31, 3);
    std::cout << "Nincs betölthető fájl";
    econio_gotoxy(31, 5);
    std::cout << "Nyomj egy gombot a visszalépéshez";
    Display::drawSquare(29, 2, 67, 7);
    while (true)
      if (econio_kbhit())
        return false;
  }
  struct dirent *entry;
  while ((entry = readdir(dir)) && totalFiles < 100) {
    const char *name = entry->d_name;
    size_t len = strlen(name);
    if (len > 4 && strcmp(name + len - 4, ".dat") == 0) {
      strncpy(fileList[totalFiles], name, 39);
      fileList[totalFiles][39] = '\0';
      totalFiles++;
    }
  }
  closedir(dir);
  if (totalFiles == 0) {
    econio_gotoxy(31, 3);
    std::cout << "Nincs betölthető fájl";
    econio_gotoxy(31, 5);
    std::cout << "Nyomj egy gombot a visszalépéshez";
    Display::drawSquare(29, 2, 67, 7);
    while (true)
      if (econio_kbhit())
        return false;
  }
#endif

  return selectFileFromList(fileList, totalFiles, filename);
}

int Display::analysisInfo(int eval) {
  econio_textcolor(COL_WHITE);
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y - 1);
  std::cout << "                               ";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y - 1);
  std::cout << "Válassz az alábbi opciók közül!";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y);
  std::cout << "Értékelés: " << eval;
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 2);
  std::cout << "< Hátra lépés";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 3);
  std::cout << "> Előre lépés";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 4);
  std::cout << "1. Alapállásra lépés";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 5);
  std::cout << "2. Végállásra lépés";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 6);
  std::cout << "3. Ugrás egy adott lépésre";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 7);
  std::cout << "4. Új variáció létrehozása";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 8);
  std::cout << "5. Ugrás variációra";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 9);
  std::cout << "X. Kilépés";
  drawSquare(74, 6, 104, 15);
  while (true) {
    if (econio_kbhit()) {
      switch (econio_getch()) {
      case KEY_LEFT:
        return 'l';
      case KEY_RIGHT:
        return 'r';
      case 'x':
      case 'X':
        return 'x';
      case '1':
        return '1';
      case '2':
        return '2';
      case '3':
        return '3';
      case '4':
        return '4';
      case '5':
        return '5';
      }
    }
  }
}

void Display::allAlternativeMoves(char (*moveArr)[5], int size) {
  for (int y = 20; y <= 23; y++) {
    econio_gotoxy(39, y);
    std::cout << "                                    ";
  }
  econio_textcolor(COL_RED);
  econio_gotoxy(40, 21);
  if (size == 0) {
    std::cout << "Nincs alternatív lépés";
    econio_textcolor(COL_RESET);
    return;
  }
  std::cout << "Összes elérhető lépés:";
  for (int i = 0; i < size; i++) {
    int column = i % 3;
    int row = i / 3;
    econio_gotoxy(40 + column * 10, 23 + row);
    std::cout << i + 1 << ". " << moveArr[i];
  }
  econio_gotoxy(60, 20);
  drawSquare(39, 20, 70, 23 + (size + 2) / 3);
  econio_textcolor(COL_RESET);
}

char Display::askPromotion() {
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 2);
  std::cout << "GYALOGÁTVÁLTOZÁS!";
  econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 4);
  std::cout << "Add meg a bábut, amivé változzon: Q R N B";
  while (true) {
    if (econio_kbhit()) {
      char inp = econio_getch();
      char result = 0;
      switch (inp) {
      case 'q':
      case 'Q':
        result = 'Q';
        break;
      case 'r':
      case 'R':
        result = 'R';
        break;
      case 'n':
      case 'N':
        result = 'N';
        break;
      case 'b':
      case 'B':
        result = 'B';
        break;
      }
      if (result != 0) {
        econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 2);
        std::cout << "                  ";
        econio_gotoxy(COORD_INFO_X, COORD_INFO_Y + 4);
        std::cout << "                                         ";
        return result;
      }
    }
  }
}

State Display::menu() {
  while (true) {
    econio_clrscr();
    econio_gotoxy(COORD_MENU_X, COORD_MENU_Y);
    std::cout << "<<Üdvözöllek a Sakk programomban!>>";
    econio_gotoxy(COORD_MENU_X + 1, COORD_MENU_Y + 2);
    std::cout << "Válassz az alábbi menüpontok közül:";
    econio_gotoxy(COORD_MENU_X - 1, COORD_MENU_Y + 3);
    std::cout << "1. Játék indítása két játékos között";
    econio_gotoxy(COORD_MENU_X - 1, COORD_MENU_Y + 4);
    std::cout << "2. Elemző mód indítása";
    econio_gotoxy(COORD_MENU_X - 1, COORD_MENU_Y + 5);
    std::cout << "X. Bezárás";
    drawSquare(29, 4, 67, 11);
    econio_rawmode();
    while (true) {
      if (econio_kbhit()) {
        char inp = econio_getch();
        switch (inp) {
        case '1':
          return STATE_GAME_MENU;
        case '2':
          return STATE_ANALYSIS_MENU;
        case 'x':
        case 'X':
          return STATE_EXIT;
        }
      }
    }
  }
}