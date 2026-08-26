#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    WINDOW *win;
    int     rows;
    int     cols;
    int     key;
    wint_t  wkey;
    int     wret;
    char   *term;
    char   *env_lines;
    char   *env_cols;

    term      = getenv("TERM");
    env_lines = getenv("LINES");
    env_cols  = getenv("COLUMNS");

    setlocale(LC_ALL, "");

    win = initscr();
    if (NULL == win) {
        printf("initscr failed\n");
        return 1;
    }

    rows = LINES;
    cols = COLS;

    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    mvprintw(0, 0, "ncurses is alive: %dx%d", cols, rows);
    mvprintw(1, 0, "press a key to continue");
    refresh();

    key  = getch();
    wret = get_wch(&wkey);

    endwin();

    printf("TERM      = %s\n", term ? term : "(unset)");
    printf("env LINES = %s\n", env_lines ? env_lines : "(unset)");
    printf("env COLS  = %s\n", env_cols ? env_cols : "(unset)");
    printf("LINES     = %d\n", rows);
    printf("COLS      = %d\n", cols);
    printf("getch     = %d\n", key);
    printf("get_wch   = %d (ret %d)\n", (int)wkey, wret);
    return 0;
}
