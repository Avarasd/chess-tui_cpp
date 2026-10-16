#ifndef ECONIO_H
#define ECONIO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum EconioColor {
    COL_BLACK = 0,
    COL_BLUE = 1,
    COL_GREEN = 2,
    COL_CYAN = 3,
    COL_RED = 4,
    COL_MAGENTA = 5,
    COL_BROWN = 6,
    COL_LIGHTGRAY = 7,
    COL_DARKGRAY = 8,
    COL_LIGHTBLUE = 9,
    COL_LIGHTGREEN = 10,
    COL_LIGHTCYAN = 11,
    COL_LIGHTRED = 12,
    COL_LIGHTMAGENTA = 13,
    COL_YELLOW = 14,    COL_LIGHTYELLOW = COL_YELLOW,
    COL_WHITE = 15,
    COL_RESET = 16,
} EconioColor;

typedef enum EconioKey {

    KEY_F1 = -1,
    KEY_F2 = -2,
    KEY_F3 = -3,
    KEY_F4 = -4,
    KEY_F5 = -5,
    KEY_F6 = -6,
    KEY_F7 = -7,
    KEY_F8 = -8,
    KEY_F9 = -9,
    KEY_F10 = -10,
    KEY_F11 = -11,
    KEY_F12 = -12,

    KEY_UP = -20,
    KEY_DOWN = -21,
    KEY_LEFT = -22,
    KEY_RIGHT = -23,
    KEY_PAGEUP = -24,
    KEY_PAGEDOWN = -25,
    KEY_HOME = -26,
    KEY_END = -27,
    KEY_INSERT = -28,
    KEY_DELETE = -29,

    KEY_CTRLUP = -30,
    KEY_CTRLDOWN = -31,
    KEY_CTRLLEFT = -32,
    KEY_CTRLRIGHT = -33,
    KEY_CTRLPAGEUP = -34,
    KEY_CTRLPAGEDOWN = -35,
    KEY_CTRLHOME = -36,
    KEY_CTRLEND = -37,
    KEY_CTRLINSERT = -38,
    KEY_CTRLDELETE = -39,

    KEY_UNKNOWNKEY = -255,

    KEY_BACKSPACE = 8,
    KEY_ENTER = 10,
    KEY_ESCAPE = 27,
    KEY_TAB = 9,
} EconioKey;

void econio_textcolor(int color);

void econio_textbackground(int color);

void econio_gotoxy(int x, int y);

void econio_clrscr();

void econio_flush();

void econio_set_title(char const *title);

void econio_rawmode();

void econio_normalmode();

bool econio_kbhit();

int econio_getch();

void econio_sleep(double sec);

#ifdef __cplusplus
} 
#endif

#endif
