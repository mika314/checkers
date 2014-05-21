#pragma once
#include <utility>
#include <vector>

typedef std::pair<int, int> Pos;
typedef std::pair<Pos, Pos> Step;
typedef std::vector<Step> Move;

class Game
{
public:
  enum Side { BLACK_SIDE = 1, WHITE_SIDE = 2 };
  enum Cell { EMPTY, BLACK = BLACK_SIDE, WHITE = WHITE_SIDE };
  Game();
  Cell cell(int x, int y) const;
  Pos selectedCell() const; 
  void selectCell(int x, int y);
  void unselect();
  void move(int x, int y);
  void getListOfLegalMoves(Side, std::vector<Move> &) const;
private:
  typedef Cell Board[8][8];
  Board board_;
  int selectedCellX_;
  int selectedCellY_;
  bool isAfterJump_;
  void findAllJumps(int x, int y, Side side, Move move, std::vector<Move> &listOfLegalMoves) const;
  bool blackMove(int level = 6);
  bool whiteMove(int level = 6);
  int score() const;
};
