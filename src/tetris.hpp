#ifndef TETRIS_HPP
#define TETRIS_HPP

typedef enum t_gamestate
{
  game = 0,
  paused,
  menu,
  options,
  end
} t_gamestate;

typedef struct
{
  string name;
  unsigned short score;
}player_score;

void logic();
void special_callback(int key,int x, int y);
void keyboard_callback(unsigned char key,int x, int y);

#endif