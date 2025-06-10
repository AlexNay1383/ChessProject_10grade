#include <stdio.h>
#include <stdlib.h>
#include "play.h"
#include "replay.h"

int main()
{
  size_t width = 8, height = 8;
  int option;

  do
  {
    printf("\n1. Start game\n");
    printf("2. Change board size (currently %zux%zu)\n", width, height);
    printf("3. Replay saved game\n");
    printf("4. Exit\n");
    printf("Choice: ");
    scanf("%d", &option);

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
      ;

    switch (option)
    {
    case 1:
      PlayGame(width, height);
      break;
    case 2:
      printf("Enter board width: ");
      scanf("%zu", &width);
      while ((c = getchar()) != '\n' && c != EOF)
        ;

      if (width < 2)
        width = 8;

      printf("Enter board height: ");
      scanf("%zu", &height);
      while ((c = getchar()) != '\n' && c != EOF)
        ;

      if (height < 2)
        height = 8;
      break;
    case 3:
    {
      char fname[100];
      printf("Enter file name for replay: ");
      scanf("%s", fname);
      while ((c = getchar()) != '\n' && c != EOF)
        ;

      ReplayGame(fname);
      break;
    }
    case 4:
      printf("Exiting...\n");
      exit(0);
      break;
    default:
      printf("Invalid choice!\n");
    }
  } while (1);

  return 0;
}
