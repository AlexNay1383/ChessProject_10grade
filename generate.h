#ifndef GENERATE_H
#define GENERATE_H

#include <stddef.h>
#include "pieces.h"

void InitializeBoard(Piece **board, size_t width, size_t height, unsigned int seed);
void PrintBoard(Piece **board, size_t width, size_t height);

#endif
