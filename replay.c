#include "replay.h"
#include "generate.h"
#include "pieces.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct
{
  char piece;
  int from_x, from_y;
  int to_x, to_y;
} Move;

static void parseMoveLine(const char *line, Move *move, int height)
{
  char from[10], to[10];
  sscanf(line, " %c %s %s", &move->piece, from, to);

  move->from_x = toupper(from[0]) - 'A';
  move->from_y = atoi(&from[1]);
  move->from_y = height - move->from_y;

  move->to_x = toupper(to[0]) - 'A';
  move->to_y = atoi(&to[1]);
  move->to_y = height - move->to_y;
}

void ReplayGame(const char *filename)
{
  size_t width;
  size_t height;
  FILE *f = fopen(filename, "r");
  if (!f)
  {
    printf("Could not open file: %s\n", filename);
    return;
  }

  unsigned int seed;
  fscanf(f, "Seed %u\n", &seed);
  fscanf(f, "Board %llu %llu\n", &width, &height);

  Piece **board = (Piece**)malloc(width * sizeof(Piece *));
  for (size_t i = 0; i < width; ++i)
    board[i] = (Piece*)malloc(height * sizeof(Piece));

  InitializeBoard(board, width, height, seed);
  printf("\n--- Replay ---\n");
  PrintBoard(board, width, height);

  char line[100];
  while (fgets(line, sizeof(line), f))
  {
    Move m;
    parseMoveLine(line, &m, height);

    if (m.piece == 'K' || m.piece == 'C')
    {
      for (size_t i = 0; i < width; ++i)
        for (size_t j = 0; j < height; ++j)
          if ((m.piece == 'K' && board[i][j] == KING) ||
              (m.piece == 'C' && board[i][j] == COMP_KING))
            board[i][j] = NONE;
    }

    board[m.to_x][m.to_y] = (m.piece == 'R') ? ROOK : (m.piece == 'K') ? KING
                                                                       : COMP_KING;
    board[m.from_x][m.from_y] = NONE;

    printf("\nMove: %s", line);
    PrintBoard(board, width, height);
    getchar();
  }

  fclose(f);
  for (size_t i = 0; i < width; ++i)
    free(board[i]);
  free(board);
}
