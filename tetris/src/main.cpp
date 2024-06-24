#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <vector>

const int BOARD_WIDTH = 20;
const int BOARD_HEIGHT = 30;

const std::vector<std::vector<std::vector<int>>> tetriminos = {
    // []
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // I
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // L
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    // J
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    // S
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // Z
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    // T
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}};

void drawBoard(const std::vector<std::vector<int>> &board) {
  const unsigned int MARGIN = 2;

  for (int y{}; y < BOARD_HEIGHT; ++y) {
    for (int x{}; x < BOARD_WIDTH; ++x) {
      if (board[y][x] == 1) {

        attron(A_BOLD);
        mvprintw(y, x * 2 + MARGIN, "[]");
        attroff(A_BOLD);
      } else if (y == 0 || y == BOARD_HEIGHT - 1 || x == 0 ||
                 x == BOARD_WIDTH - 1) {
        attron(A_BOLD);
        mvprintw(y, x * 2 + MARGIN, "*");
        attroff(A_BOLD);
      } else {
        mvprintw(y, x * 2 + MARGIN, ".");
      }
    }
  }
}

void placeTetrimino(const std::vector<std::vector<int>> &tetrimino,
                    std::vector<std::vector<int>> &board, int y, int x) {

  for (int dy{}; dy < 4; ++dy) {
    for (int dx{}; dx < 4; ++dx) {
      if (tetrimino[dy][dx] == 1) {
        board[y + dy][x + dx] = 1;
      }
    }
  }
}

void clearTetrimino(const std::vector<std::vector<int>> &tetrimino,
                    std::vector<std::vector<int>> &board, int y, int x) {

  for (int dy{}; dy < 4; ++dy) {
    for (int dx{}; dx < 4; ++dx) {
      if (tetrimino[dy][dx] == 1) {
        board[y + dy][x + dx] = 0;
      }
    }
  }
}

bool canPlaceTetrimino(const std::vector<std::vector<int>> &tetrimino,
                       const std::vector<std::vector<int>> &board, int y,
                       int x) {

  if (y < 0) {
    return false;
  }

  for (int dy{}; dy < 4; ++dy) {
    for (int dx{}; dx < 4; ++dx) {
      if (tetrimino[dy][dx] == 1 &&
          (board[y + dy][x + dx] == 1 || y + dy == BOARD_HEIGHT - 1 ||
           x + dx == 0 || x + dx == BOARD_WIDTH - 1)) {
        return false;
      }
    }
  }

  return true;
}

void rotateTetromino(std::vector<std::vector<int>> &tetrimino) {
  std::vector<std::vector<int>> temp(4, std::vector<int>(4, 0));
  for (int i{}; i < 4; ++i) {
    for (int j{}; j < 4; ++j) {
      temp[i][j] = tetrimino[3 - j][i];
    }
  }

  for (int i{}; i < 4; ++i) {
    for (int j{}; j < 4; ++j) {
      tetrimino[i][j] = temp[i][j];
    }
  }
}

void removeCompletedLines(std::vector<std::vector<int>> &board, int &score) {
  for (int y = BOARD_HEIGHT - 2; y > 0; --y) {
    bool isLineComplete = true;
    for (int x = 1; x < BOARD_WIDTH - 1; ++x) {
      if (board[y][x] == 0) {
        isLineComplete = false;
        break;
      }
    }
    if (isLineComplete) {
      for (int i = y; i > 0; --i) {
        for (int j = 1; j < BOARD_WIDTH - 1; ++j) {
          board[i][j] = board[i - 1][j];
        }
      }
      for (int j = 1; j < BOARD_WIDTH - 1; ++j) {
        board[0][j] = 0;
      }
      ++y;
      score += 100;
    }
  }
}

int main() {
  initscr();
  noecho();
  curs_set(0);
  cbreak();
  keypad(stdscr, true);
  nodelay(stdscr, true);

  std::vector<std::vector<int>> board(BOARD_HEIGHT,
                                      std::vector<int>(BOARD_WIDTH, 0));

  srand(time(nullptr));
  int y = 0, x = 5, speed = 150, score = 0;
  bool canShowNextTetrimino = false;
  auto tetrimino = tetriminos[rand() % tetriminos.size()];

  while (true) {

    clear();
    drawBoard(board);
    attron(A_REVERSE);
    mvprintw(LINES - 1, 0, "Score: %d", score);
    attroff(A_REVERSE);
    refresh();

    clearTetrimino(tetrimino, board, y, x);

    if (canPlaceTetrimino(tetrimino, board, y + 1, x)) {
      ++y;
    } else if (y == 0) {
      attron(A_REVERSE);
      mvprintw(LINES / 2, (COLS / 2) - 4, "GAME OVER");
      attroff(A_REVERSE);
      refresh();
      nodelay(stdscr, false);
      getch();
      printf("Your last score was: %d", score);
      break;
    } else {
      canShowNextTetrimino = true;
    }

    placeTetrimino(tetrimino, board, y, x);
    removeCompletedLines(board, score);

    int ch = getch();

    if (ch == 'q') {
      break;
    }

    napms(speed);

    clearTetrimino(tetrimino, board, y, x);
    switch (ch) {
    case KEY_RIGHT:
      if (canPlaceTetrimino(tetrimino, board, y, x + 1)) {
        ++x;
      }
      break;

    case KEY_LEFT:
      if (canPlaceTetrimino(tetrimino, board, y, x - 1)) {
        --x;
      }
      break;

    case KEY_UP:
      rotateTetromino(tetrimino);
      break;

    case KEY_DOWN:
      speed = 50;
    }
    placeTetrimino(tetrimino, board, y, x);
    removeCompletedLines(board, score);

    if (canShowNextTetrimino) {
      y = 0, x = 5;
      canShowNextTetrimino = false;
      speed = 150;
      tetrimino = tetriminos[rand() % tetriminos.size()];
    }
  }

  endwin();
  return 0;
}
