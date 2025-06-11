#include "play.h"
#include "generate.h"
#include "pieces.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_MOVES 1000

typedef struct
{
  char piece;
  int from_x, from_y;
  int to_x, to_y;
} RecordedMove;

typedef struct
{
  RecordedMove moves[MAX_MOVES];
  int count;
  int rookMoves, kingMoves, compKingMoves;
  int checks;
} GameStats;

typedef struct
{
  int from_x, from_y;
  int to_x, to_y;
} Move;

static int parseMove(const char *from, const char *to, Move *move, int height)
{
  if (!isalpha(from[0]) || !isdigit(from[1]))
    return 0;
  if (!isalpha(to[0]) || !isdigit(to[1]))
    return 0;

  move->from_x = toupper(from[0]) - 'A';
  move->from_y = atoi(&from[1]);
  move->from_y = height - move->from_y;

  move->to_x = toupper(to[0]) - 'A';
  move->to_y = atoi(&to[1]);
  move->to_y = height - move->to_y;

  return 1;
}

int isSafeForCompKing(Piece **board, size_t width, size_t height, int nx, int ny)
{
  int playerKingX = -1, playerKingY = -1;
  for (size_t i = 0; i < width; ++i)
    for (size_t j = 0; j < height; ++j)
      if (board[i][j] == KING)
      {
        playerKingX = i;
        playerKingY = j;
        break;
      }

  if (abs(nx - playerKingX) <= 1 && abs(ny - playerKingY) <= 1)
    return 0;

  for (int x = nx - 1; x >= 0; --x)
  {
    if (board[x][ny] != NONE && board[x][ny] != COMP_KING)
    {
      if (board[x][ny] == ROOK)
        return 0;
      break;
    }
  }
  for (int x = nx + 1; x < (int)width; ++x)
  {
    if (board[x][ny] != NONE && board[x][ny] != COMP_KING)
    {
      if (board[x][ny] == ROOK)
        return 0;
      break;
    }
  }
  for (int y = ny - 1; y >= 0; --y)
  {
    if (board[nx][y] != NONE && board[nx][y] != COMP_KING)
    {
      if (board[nx][y] == ROOK)
        return 0;
      break;
    }
  }
  for (int y = ny + 1; y < (int)height; ++y)
  {
    if (board[nx][y] != NONE && board[nx][y] != COMP_KING)
    {
      if (board[nx][y] == ROOK)
        return 0;
      break;
    }
  }

  return 1;
}

static int isCheck(Piece **board, size_t width, size_t height)
{

  int kingX = -1, kingY = -1;
  for (size_t i = 0; i < width; ++i)
  {
    for (size_t j = 0; j < height; ++j)
    {
      if (board[i][j] == COMP_KING)
      {
        kingX = i;
        kingY = j;
        break;
      }
    }
  }

  for (int x = kingX - 1; x >= 0; --x)
  {
    if (board[x][kingY] != NONE && board[x][kingY] != COMP_KING)
    {
      if (board[x][kingY] == ROOK)
        return 1;
      else
        break;
    }
  }

  for (int x = kingX + 1; x < (int)width; ++x)
  {
    if (board[x][kingY] != NONE && board[x][kingY] != COMP_KING)
    {
      if (board[x][kingY] == ROOK)
        return 1;
      else
        break;
    }
  }

  for (int y = kingY - 1; y >= 0; --y)
  {
    if (board[kingX][y] != NONE && board[kingX][y] != COMP_KING)
    {
      if (board[kingX][y] == ROOK)
        return 1;
      else
        break;
    }
  }

  for (int y = kingY + 1; y < (int)height; ++y)
  {
    if (board[kingX][y] != NONE && board[kingX][y] != COMP_KING)
    {
      if (board[kingX][y] == ROOK)
        return 1;
      else
        break;
    }
  }

  return 0;
}

static int isCheckmate(Piece **board, size_t width, size_t height)
{
  if (!isCheck(board, width, height))
    return 0;

  int cx = -1, cy = -1;
  for (size_t i = 0; i < width; ++i)
    for (size_t j = 0; j < height; ++j)
      if (board[i][j] == COMP_KING)
      {
        cx = i;
        cy = j;
        break;
      }

  int dirs[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  for (int d = 0; d < 8; d++)
  {
    int nx = cx + dirs[d][0];
    int ny = cy + dirs[d][1];
    if (nx < 0 || ny < 0 || nx >= (int)width || ny >= (int)height)
      continue;

    if (isSafeForCompKing(board, width, height, nx, ny))
      return 0;
  }

  return 1;
}

static int isStalemate(Piece **board, size_t width, size_t height)
{
  int cx = -1, cy = -1;
  for (size_t i = 0; i < width; ++i)
    for (size_t j = 0; j < height; ++j)
      if (board[i][j] == COMP_KING)
      {
        cx = i;
        cy = j;
        break;
      }

  if (isCheck(board, width, height))
    return 0;

  int dirs[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  for (int d = 0; d < 8; d++)
  {
    int nx = cx + dirs[d][0];
    int ny = cy + dirs[d][1];
    if (nx < 0 || ny < 0 || nx >= (int)width || ny >= (int)height)
      continue;

    if (isSafeForCompKing(board, width, height, nx, ny))
      return 0;
  }
  return 1;
}

static int isOnlyKings(Piece **board, size_t width, size_t height)
{
  int kingCount = 0;
  for (size_t i = 0; i < width; ++i)
  {
    for (size_t j = 0; j < height; ++j)
    {
      if (board[i][j] == KING || board[i][j] == COMP_KING)
        kingCount++;
      else if (board[i][j] != NONE)
        return 0;
    }
  }
  return kingCount == 2;
}

static void ComputerMove(Piece **board, size_t width, size_t height, GameStats *stats)
{
  int cx = -1, cy = -1;
  for (size_t i = 0; i < width; ++i)
    for (size_t j = 0; j < height; ++j)
      if (board[i][j] == COMP_KING)
      {
        cx = i;
        cy = j;
        break;
      }

  int dirs[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  double centerX = (width - 1) / 2.0;
  double centerY = (height - 1) / 2.0;
  double bestDist = 100000.0;
  int bestX = cx, bestY = cy;
  int foundCapture = 0;

  for (int d = 0; d < 8; d++)
  {
    int nx = cx + dirs[d][0];
    int ny = cy + dirs[d][1];
    if (nx < 0 || ny < 0 || nx >= (int)width || ny >= (int)height)
      continue;

    int playerKingX = -1, playerKingY = -1;
    for (size_t i2 = 0; i2 < width; ++i2)
      for (size_t j2 = 0; j2 < height; ++j2)
        if (board[i2][j2] == KING)
        {
          playerKingX = i2;
          playerKingY = j2;
          break;
        }
    if (abs(nx - playerKingX) <= 1 && abs(ny - playerKingY) <= 1)
      continue;

    int safe = 1;
    for (int x = nx - 1; x >= 0; --x)
    {
      if (board[x][ny] != NONE && board[x][ny] != COMP_KING)
      {
        if (board[x][ny] == ROOK)
          safe = 0;
        break;
      }
    }
    for (int x = nx + 1; x < (int)width; ++x)
    {
      if (board[x][ny] != NONE && board[x][ny] != COMP_KING)
      {
        if (board[x][ny] == ROOK)
          safe = 0;
        break;
      }
    }
    for (int y = ny - 1; y >= 0; --y)
    {
      if (board[nx][y] != NONE && board[nx][y] != COMP_KING)
      {
        if (board[nx][y] == ROOK)
          safe = 0;
        break;
      }
    }
    for (int y = ny + 1; y < (int)height; ++y)
    {
      if (board[nx][y] != NONE && board[nx][y] != COMP_KING)
      {
        if (board[nx][y] == ROOK)
          safe = 0;
        break;
      }
    }

    if (!safe)
      continue;

    if (board[nx][ny] == ROOK)
    {
      board[nx][ny] = COMP_KING;
      board[cx][cy] = NONE;
      stats->moves[stats->count++] = (RecordedMove){'C', cx, cy, nx, ny};
      stats->compKingMoves++;
      return;
    }

    if (board[nx][ny] == NONE)
    {
      double dist = (nx - centerX) * (nx - centerX) + (ny - centerY) * (ny - centerY);
      if (dist < bestDist)
      {
        bestDist = dist;
        bestX = nx;
        bestY = ny;
      }
    }
  }

  if (bestX != cx || bestY != cy)
  {
    board[bestX][bestY] = COMP_KING;
    board[cx][cy] = NONE;
    stats->moves[stats->count++] = (RecordedMove){'C', cx, cy, bestX, bestY};
    stats->compKingMoves++;
  }
}

void PlayGame(size_t width, size_t height)
{
  unsigned int seed = (unsigned int)time(NULL);
  Piece **board = malloc(width * sizeof(Piece *));
  for (size_t i = 0; i < width; ++i)
    board[i] = malloc(height * sizeof(Piece));

  InitializeBoard(board, width, height, seed);

  GameStats stats = {.count = 0, .rookMoves = 0, .kingMoves = 0, .compKingMoves = 0, .checks = 0};

  while (1)
  {
    printf("\n\n");
    PrintBoard(board, width, height);

    if (isCheckmate(board, width, height))
    {
      printf("\nCheckmate! You win!\n");
      break;
    }

    if (isOnlyKings(board, width, height))
    {
      printf("\nDraw by insufficient material.\n");
      break;
    }

    if (isStalemate(board, width, height))
    {
      printf("\nStalemate! Game is drawn.\n");
      break;
    }

    printf("\nInput your move: ");
    char from[10], to[10];
    scanf("%s %s", from, to);

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
      ;

    Move move;
    if (!parseMove(from, to, &move, height))
    {
      printf("Invalid input format!\n");
      continue;
    }

    if (move.from_x < 0 || move.from_x >= (int)width || move.from_y < 0 || move.from_y >= (int)height ||
        move.to_x < 0 || move.to_x >= (int)width || move.to_y < 0 || move.to_y >= (int)height)
    {
      printf("Invalid move: out of board.\n");
      continue;
    }

    if (move.from_x == move.to_x && move.from_y == move.to_y)
    {
      printf("Invalid move: must move to a different square.\n");
      continue;
    }

    if (board[move.from_x][move.from_y] == NONE)
    {
      printf("No piece at source!\n");
      continue;
    }

    if (board[move.from_x][move.from_y] == COMP_KING)
    {
      printf("You can't move the computer's king!\n");
      continue;
    }

    if (board[move.to_x][move.to_y] == COMP_KING)
    {
      printf("You can't move onto computer's king!\n");
      continue;
    }

    if (board[move.to_x][move.to_y] == ROOK || board[move.to_x][move.to_y] == KING)
    {
      printf("Invalid move: can't move onto your own piece!\n");
      continue;
    }

    Piece movingPiece = board[move.from_x][move.from_y];

    if (movingPiece == KING)
    {
      int dx = abs(move.to_x - move.from_x);
      int dy = abs(move.to_y - move.from_y);
      if (dx > 1 || dy > 1)
      {
        printf("Invalid move for King: can move only 1 cell!\n");
        continue;
      }

      int compKingX = -1, compKingY = -1;
      for (size_t i = 0; i < width; ++i)
        for (size_t j = 0; j < height; ++j)
          if (board[i][j] == COMP_KING)
          {
            compKingX = i;
            compKingY = j;
            break;
          }
      if (abs(move.to_x - compKingX) <= 1 && abs(move.to_y - compKingY) <= 1)
      {
        printf("Invalid move: kings cannot be adjacent!\n");
        continue;
      }
    }

    if (movingPiece == ROOK)
    {
      if (move.from_x != move.to_x && move.from_y != move.to_y)
      {
        printf("Invalid move for Rook: can move only in straight line!\n");
        continue;
      }

      int blocked = 0;
      if (move.from_x == move.to_x)
      {
        int minY = move.from_y < move.to_y ? move.from_y : move.to_y;
        int maxY = move.from_y > move.to_y ? move.from_y : move.to_y;
        for (int y = minY + 1; y < maxY; ++y)
        {
          if (board[move.from_x][y] != NONE)
          {
            blocked = 1;
            break;
          }
        }
      }
      else
      {
        int minX = move.from_x < move.to_x ? move.from_x : move.to_x;
        int maxX = move.from_x > move.to_x ? move.from_x : move.to_x;
        for (int x = minX + 1; x < maxX; ++x)
        {
          if (board[x][move.from_y] != NONE)
          {
            blocked = 1;
            break;
          }
        }
      }
      if (blocked)
      {
        printf("Invalid move for Rook: path is blocked!\n");
        continue;
      }
    }

    board[move.to_x][move.to_y] = movingPiece;
    board[move.from_x][move.from_y] = NONE;

    stats.moves[stats.count++] = (RecordedMove){movingPiece == ROOK ? 'R' : 'K',
                                                move.from_x, move.from_y, move.to_x, move.to_y};
    if (movingPiece == ROOK)
      stats.rookMoves++;
    else if (movingPiece == KING)
      stats.kingMoves++;

    if (isCheck(board, width, height))
      stats.checks++;

    ComputerMove(board, width, height, &stats);
  }

  printf("\n--- Game Over ---\n");
  printf("Seed: %u\n", seed);
  printf("Total moves: %d\n", stats.count);
  printf("Rook moves: %d\n", stats.rookMoves);
  printf("King moves: %d\n", stats.kingMoves);
  printf("Computer king moves: %d\n", stats.compKingMoves);
  printf("Number of checks: %d\n", stats.checks);

  char filename[100];
  printf("\nEnter filename to save the game: ");
  scanf("%99s", filename);

  FILE *f = fopen(filename, "w");
  if (!f)
  {
    printf("Error opening file!\n");
  }
  else
  {
    fprintf(f, "Seed %u\n", seed);
    fprintf(f, "Board %zu %zu\n", width, height);
    for (int i = 0; i < stats.count; i++)
    {
      fprintf(f, "%c %c%d %c%d\n",
              stats.moves[i].piece,
              'A' + stats.moves[i].from_x, (int)(height - stats.moves[i].from_y),
              'A' + stats.moves[i].to_x, (int)(height - stats.moves[i].to_y));
    }
    fclose(f);
    printf("Game saved to '%s'. Returning to menu.\n", filename);
  }

  for (size_t i = 0; i < width; ++i)
    free(board[i]);
  free(board);
}
