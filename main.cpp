#include <GL/glut.h>
#include "game.hpp"
#include <cmath>

Game game;

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  for (int x = 0; x < 8; ++x)
    for (int y = 0; y < 8; ++y)
    {
      if ((x + y) % 2)
        glColor3f(0.0f, 0.0f, 0.0f);
      else
        glColor3f(1.0f, 1.0f, 1.0f);
      glBegin(GL_QUADS);
      glVertex2f(x * 480 / 8, y * 480 / 8);
      glVertex2f((x + 1) * 480 / 8, y * 480 / 8);
      glVertex2f((x + 1) * 480 / 8, (y + 1) * 480 / 8);
      glVertex2f(x * 480 / 8, (y + 1) * 480 / 8);
      glEnd();
      switch (game.cell(x, y))
      {
      case Game::EMPTY:
        break;
      case Game::BLACK:
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_POLYGON);
        for (int a = 0; a < 5; ++a)
        {
          float xx = 480 / 8 / 2 * cos(2 * 3.1415926 * a / 5) + (x + 0.5f) * 480 / 8;
          float yy =  480 / 8 / 2 * sin(2 * 3.1415926 * a / 5) + (y + 0.5f) * 480 / 8;
          glVertex2f(xx, yy);
        }
        glEnd();
        break;
      case Game::WHITE:
        Pos selected = game.selectedCell();
        if (selected.first == x && selected.second == y)
          glColor3f(1.0f, 0.0f, 0.0f);
        else
          glColor3f(0.7f, 0.7f, 0.7f);
        glBegin(GL_POLYGON);
        for (int a = 0; a < 5; ++a)
        {
          float xx = 480 / 8 / 2 * cos(2 * 3.1415926 * a / 5) + (x + 0.5f) * 480 / 8;
          float yy =  480 / 8 / 2 * sin(2 * 3.1415926 * a / 5) + (y + 0.5f) * 480 / 8;
          glVertex2f(xx, yy);
        }
        glEnd();
        break;
      }
    }
#ifdef DEBUG
  std::vector<Move> moves;
  game.getListOfLegalMoves(Game::WHITE_SIDE, moves);
  glColor3f(0.0f, 1.0f, 0.0f);
  glBegin(GL_LINES);
  for (std::vector<Move>::iterator i = moves.begin(); i != moves.end(); ++i)
  {
    for (Move::iterator j = i->begin(); j != i->end(); ++j)
    {
      int x1 = j->first.first;
      int y1 = j->first.second;
      int x2 = j->second.first;
      int y2 = j->second.second;
      glVertex2f((x1 + 0.5f) * 480 / 8, (y1 + 0.5f) * 480 / 8);
      glVertex2f((x2 + 0.5f) * 480 / 8, (y2 + 0.5f) * 480 / 8);
    }
  }
#endif
  glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
  if (state == GLUT_UP)
  {
    std::vector<Move> moves;
    game.getListOfLegalMoves(Game::WHITE_SIDE, moves);
    if (moves.empty())
      game = Game();
    const int xx = x / (480 / 8);
    const int yy = y / (480 / 8);
    if (game.selectedCell() == std::make_pair(-1, -1))
      game.selectCell(xx, yy);
    else
      game.move(xx, yy);
    glutPostRedisplay();
  }
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(480, 480); 
  glutInitWindowPosition(0, 95);
  glutCreateWindow("Checkers");
  glClearColor(0, 0, 0, 1);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 480, 480, 0, -1.0, 1.0);
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMainLoop();
}
