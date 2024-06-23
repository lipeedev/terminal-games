#include <cstdlib>
#include <deque>
#include <ncurses.h>
#include <unistd.h>

struct Point {
  int x, y;
};

int main() {
  initscr();
  cbreak();
  noecho();
  curs_set(false);
  keypad(stdscr, true);
  nodelay(stdscr, true);

  int max_x = getmaxx(stdscr), max_y = getmaxy(stdscr);

  std::deque<Point> snake = {{3, 0}, {2, 0}, {1, 0}, {0, 0}};
  Point dir = {1, 0}, food = {10, 10};
  int score = 0, speed_count = 0;

  while (true) {
    int ch = getch();

    if (ch == 'q')
      break;

    switch (ch) {
    case KEY_UP:
      if (dir.y != 1)
        dir = {0, -1};
      break;
    case KEY_DOWN:
      if (dir.y != -1)
        dir = {0, 1};
      break;
    case KEY_LEFT:
      if (dir.x != 1)
        dir = {-1, 0};
      break;
    case KEY_RIGHT:
      if (dir.x != -1)
        dir = {1, 0};
      break;
    }

    clear();

    mvprintw(food.y, food.x, "@");
    mvprintw(LINES - 1, 0, "SCORE: %d", score);

    Point new_head = {snake.front().x + dir.x, snake.front().y + dir.y};

    for (auto &p : snake) {
      if (p.x == new_head.x && p.y == new_head.y) {
        attron(A_REVERSE);
        mvprintw(LINES / 2, (COLS / 2) - 4, "GAME OVER");
        attroff(A_REVERSE);
        refresh();
        nodelay(stdscr, false);
        getch();
        printf("Your last score was: %d", score);
        endwin();
        return 0;
      }
      mvprintw(p.y, p.x, "$");
    }

    refresh();
    usleep(100000 - speed_count);

    if (new_head.x >= max_x)
      new_head.x = 0;
    if (new_head.x < 0)
      new_head.x = max_x - 1;
    if (new_head.y >= max_y)
      new_head.y = 0;
    if (new_head.y < 0)
      new_head.y = max_y - 1;

    snake.push_front(new_head);
    snake.pop_back();

    if (new_head.x == food.x && new_head.y == food.y) {
      snake.push_back(snake.back());
      food = {rand() % max_x, rand() % max_y};
      score += 10;
      speed_count += (score < 150) ? (score * 20) : (score * 10);
    }
  }

  endwin();

  return 0;
}
