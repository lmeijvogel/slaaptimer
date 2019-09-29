#pragma once

#include <ncurses.h>
#include <string>

class NCursesGui {
public:
  NCursesGui() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    refresh();
  }

  ~NCursesGui() {
    endwin();
  }

  void print(std::string line, int position) {
    print(line.c_str(), position);
  }
  void print(const char *line, int position) {
    mvprintw(position, 0, line);
    refresh();
  }

  void print(char *line, int position) {
    mvprintw(position, 0, line);

    refresh();
  }
};


