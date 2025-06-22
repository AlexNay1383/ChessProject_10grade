#include "generate.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void InitializeBoard(Piece **board, size_t width, size_t height, unsigned int seed)
{
  srand(seed);
  do
  {
    for (size_t i = 0; i < width; ++i)
      for (size_t j = 0; j < height; ++j)
        board[i][j] = NONE;

    size_t totalCells = width * height;
    int used[totalCells];
    for (size_t i = 0; i < totalCells; ++i)
      used[i] = 0;

    int idx;
    do
    {
      idx = rand() % totalCells;
    } while (used[idx]);
    used[idx] = 1;
    board[idx % width][idx / width] = ROOK;

    do
    {
      idx = rand() % totalCells;
    } while (used[idx]);
    used[idx] = 1;
    board[idx % width][idx / width] = ROOK;

    do
    {
      idx = rand() % totalCells;
    } while (used[idx]);
    used[idx] = 1;
    board[idx % width][idx / width] = KING;

    do
    {
      idx = rand() % totalCells;
    } while (used[idx]);
    used[idx] = 1;
    board[idx % width][idx / width] = COMP_KING;

    int kingX = -1, kingY = -1, compKingX = -1, compKingY = -1;
    for (size_t i = 0; i < width; ++i)
      for (size_t j = 0; j < height; ++j)
      {
        if (board[i][j] == KING)
        {
          kingX = i;
          kingY = j;
        }
        else if (board[i][j] == COMP_KING)
        {
          compKingX = i;
          compKingY = j;
        }
      }

    int kingsAdjacent = abs(kingX - compKingX) <= 1 && abs(kingY - compKingY) <= 1;

    int inCheck = 0;
    for (size_t i = 0; i < width; ++i)
      if (board[i][compKingY] == ROOK)
        inCheck = 1;
    for (size_t j = 0; j < height; ++j)
      if (board[compKingX][j] == ROOK)
        inCheck = 1;

    if (!kingsAdjacent && !inCheck)
      break;

  } while (1);
}

//Krystev

// void PrintBoard(Piece **board, size_t width, size_t height)
// {
//   printf("  ");
//   for (size_t i = 0; i < width; ++i)
//     printf(" %c", (int)('A' + i));
//   printf("\n");

//   for (size_t j = 0; j < height; ++j)
//   {
//     printf("%2u", height - j);
//     for (size_t i = 0; i < width; ++i)
//     {
//       char c = '.';
//       switch (board[i][j])
//       {
//       case ROOK:
//         c = 'R';
//         break;
//       case KING:
//         c = 'K';
//         break;
//       case COMP_KING:
//         c = 'C';
//         break;
//       default:
//         c = '.';
//         break;
//       }
//       printf(" %c", c);
//     }
//     printf(" %llu", height - j);
//     printf("\n");
//   }

//   printf("  ");
//   for (size_t i = 0; i < width; ++i)
//     printf(" %c", (int)('A' + i));
//   printf("\n");
// }
