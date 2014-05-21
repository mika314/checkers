#include "game.hpp"
#include <cassert>
#include <cstdlib>

Game::Game():
  selectedCellX_(-1),
  selectedCellY_(-1),
  isAfterJump_(false)
{
  for (int x = 0; x < 8; ++x)
    for (int y = 0; y < 8; ++y)
      board_[x][y] = EMPTY;
  for (int y = 0; y < 3; ++y)
    for (int x = (y + 1) % 2; x < 8; x += 2)
      board_[x][y] = BLACK;
  for (int y = 5; y < 8; ++y)
    for (int x = (y + 1) % 2; x < 8; x += 2)
      board_[x][y] = WHITE;
                               
}

Game::Cell Game::cell(int x, int y) const
{
  return board_[x][y];
}

Pos Game::selectedCell() const
{
  return std::make_pair(selectedCellX_, selectedCellY_);
}

void Game::selectCell(int x, int y)
{
  if (x < 8 && x >= 0 && y >= 0 && y < 8 && board_[x][y] == WHITE)
  {
    selectedCellX_ = x;
    selectedCellY_ = y;
  }
  else
  {
    selectedCellX_ = -1;
    selectedCellY_ = -1;
  }
}

void Game::move(int x, int y)
{
  assert(selectedCellX_ != -1);
  assert(selectedCellY_ != -1);

  std::vector<Move> listOfLegalMoves;
  getListOfLegalMoves(WHITE_SIDE, listOfLegalMoves);
  if (listOfLegalMoves.empty())
  {
    *this = Game();
    return;
  }

  Step step = Step(Pos(selectedCellX_, selectedCellY_), Pos(x, y));

  bool moved = false;

  for (std::vector<Move>::iterator i = listOfLegalMoves.begin(); i != listOfLegalMoves.end(); ++i)
  {
    if (step == i->front() && (!isAfterJump_ || (isAfterJump_ && abs(selectedCellX_ - x) == 2)))
    {
      moved = true;
      board_[selectedCellX_][selectedCellY_] = EMPTY;
      board_[x][y] = WHITE;
      if (abs(selectedCellX_ - x) == 2)
      {
        board_[(selectedCellX_ + x) / 2][(selectedCellY_ + y) / 2] = EMPTY;
        selectedCellX_ = x;
        selectedCellY_ = y;
        isAfterJump_ = true;
        return;
      }
      else
        isAfterJump_ = false;
      break;
    }
  }
  
  if (isAfterJump_ || moved)
  {
    isAfterJump_ = false;
    blackMove();
  }
  selectedCellX_ = -1;
  selectedCellY_ = -1;
}

void Game::getListOfLegalMoves(Side side, std::vector<Move> &lolm) const
{
  // checking for jump
  for (int x = 0; x < 8; ++x)
    for (int y = 0; y < 8; ++y)
      if (board_[x][y] == static_cast<Cell>(side))
      {
        Move move;
        findAllJumps(x, y, side, move, lolm);
      }
  // jumping is mandatory
  if (lolm.empty())
  {
    // checking for move
    for (int x = 0; x < 8; ++x)
      for (int y = 0; y < 8; ++y)
        if (board_[x][y] == static_cast<Cell>(side))
        {
          int yy = y + (side == WHITE_SIDE ? -1 : 1);
          if (yy >= 0 && yy < 8)
            for (int xx = x - 1; xx <= x + 1; xx += 2)
            {
              if (xx < 0)
                continue;
              if (xx >= 8)
                continue;
              if (board_[xx][yy] == EMPTY)
              {
                Move move;
                move.push_back(Step(Pos(x, y), Pos(xx, yy)));
                lolm.push_back(move);
              }
            }
        }
  }
}

void Game::findAllJumps(int x, int y, Side side, Move move, std::vector<Move> &lolm) const
{
  const int dx[] = {-1, 1, 1, -1};
  const int dy[] = {-1, -1, 1, 1};
  for (int d = 0; d < 4; ++d)
  {
    const int xx = x + dx[d];
    const int yy = y + dy[d];
    if (xx < 0 || xx >= 8 || yy < 0 || yy >= 8)
      continue;
    if (board_[xx][yy] == EMPTY || board_[xx][yy] == static_cast<Cell>(side))
      continue;
    const int xxx = x + 2 * dx[d];
    const int yyy = y + 2 * dy[d];
    if (xxx < 0 || xxx >= 8 || yyy < 0 || yyy >= 8)
      continue;
    if (board_[xxx][yyy] == EMPTY)
    {
      bool samePlace = false;
      for (Move::iterator i = move.begin(); i != move.end(); ++i)
        if (i->second == Pos(xxx, yyy))
        {
          samePlace = true;
          break;
        }
      if (!samePlace)
      {
        move.push_back(Step(Pos(x, y), Pos(xxx, yyy)));
        lolm.push_back(move);
        findAllJumps(xxx, yyy, side, move, lolm);
      }
    }
  }
}

bool Game::blackMove(int level)
{
  std::vector<Move> listOfLegalMoves;
  getListOfLegalMoves(BLACK_SIDE, listOfLegalMoves);
  if (listOfLegalMoves.empty())
  {
    *this = Game();
    return false;
  }
  int maxScore = -20;
  std::vector<Move>::iterator bestMove = listOfLegalMoves.end();
  for (std::vector<Move>::iterator move = listOfLegalMoves.begin(); move != listOfLegalMoves.end(); ++move)
  {
    Game game = *this;
    for (Move::iterator i = move->begin(); i != move->end(); ++i)
    {
      game.board_[i->first.first][i->first.second] = EMPTY;
      game.board_[i->second.first][i->second.second] = BLACK;
      if (abs(i->second.first - i->first.first) == 2)
        game.board_[(i->first.first + i->second.first) / 2][(i->first.second + i->second.second) / 2] = EMPTY;
    }
    if (level == 0 || game.whiteMove(level - 1))
    {
      int tmpScore = game.score();
      if (maxScore < tmpScore)
      {
        maxScore = tmpScore;
        bestMove = move;
      }
    } 
    else
    {
      maxScore = 100;
      bestMove = move;
    }

  }
  for (Move::iterator i = bestMove->begin(); i != bestMove->end(); ++i)
  {
    board_[i->first.first][i->first.second] = EMPTY;
    board_[i->second.first][i->second.second] = BLACK;
    if (abs(i->second.first - i->first.first) == 2)
      board_[(i->first.first + i->second.first) / 2][(i->first.second + i->second.second) / 2] = EMPTY;
  }
  return true;
}

bool Game::whiteMove(int level)
{
  std::vector<Move> listOfLegalMoves;
  getListOfLegalMoves(WHITE_SIDE, listOfLegalMoves);
  if (listOfLegalMoves.empty())
  {
    *this = Game();
    return false;
  }
  int maxScore = 20;
  std::vector<Move>::iterator bestMove = listOfLegalMoves.end();
  for (std::vector<Move>::iterator move = listOfLegalMoves.begin(); move != listOfLegalMoves.end(); ++move)
  {
    Game game = *this;
    for (Move::iterator i = move->begin(); i != move->end(); ++i)
    {
      game.board_[i->first.first][i->first.second] = EMPTY;
      game.board_[i->second.first][i->second.second] = WHITE;
      if (abs(i->second.first - i->first.first) == 2)
        game.board_[(i->first.first + i->second.first) / 2][(i->first.second + i->second.second) / 2] = EMPTY;
    }
    if (level == 0 || game.blackMove(level - 1))
    {
      int tmpScore = game.score();
      if (maxScore > tmpScore)
      {
        maxScore = tmpScore;
        bestMove = move;
      }
    } 
    else
    {
      maxScore = -100;
      bestMove = move;
    }
  }
  for (Move::iterator i = bestMove->begin(); i != bestMove->end(); ++i)
  {
    board_[i->first.first][i->first.second] = EMPTY;
    board_[i->second.first][i->second.second] = WHITE;
    if (abs(i->second.first - i->first.first) == 2)
      board_[(i->first.first + i->second.first) / 2][(i->first.second + i->second.second) / 2] = EMPTY;
  }
  return true;
}

int Game::score() const
{
  int res = 0;
  for (int x = 0; x < 8; ++x)
    for (int y = 0; y < 8; ++y)
      if (board_[x][y] == BLACK)
        ++res;
      else if (board_[x][y] == WHITE)
        --res;
  return res;
}
