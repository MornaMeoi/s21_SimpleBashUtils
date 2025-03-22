#ifndef S21_CAT
#define S21_CAT

#include <stdbool.h>
#include <stdio.h>

typedef struct {
  bool number_non_blank;
  bool mark_endl;
  bool number_all;
  bool squeeze;
  bool tab;
  bool print_non_printable;
} Flags;

Flags CatReadFlags(int argc, char **argv);
void CatFile(FILE *file, Flags flags, const char *table[256], int *file_num);
void Cat(int argc, char *argv[], Flags flags, const char *table[static 256]);

#endif