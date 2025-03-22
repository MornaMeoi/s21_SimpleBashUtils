#include "s21_cat.h"

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "cat_flags.h"

int main(int argc, char *argv[]) {
  Flags flags = CatReadFlags(argc, argv);
  const char *table[256];
  CatSetTable(table);
  if (flags.mark_endl) CatSetEndl(table);
  if (flags.tab) CatSetTab(table);
  if (flags.print_non_printable) CatSetNonPrintable(table);
  Cat(argc, argv, flags, table);
  return 0;
}

Flags CatReadFlags(int argc, char *argv[]) {
  struct option long_options[] = {{"number-nonblank", 0, NULL, 'b'},
                                  {"number", 0, NULL, 'n'},
                                  {"squeeze-blank", 0, NULL, 's'}};
  int current_flag = 0;
  Flags flags = {false, false, false, false, false, false};
  while (current_flag != -1) {
    current_flag = getopt_long(argc, argv, "bevEnstT", long_options, NULL);
    switch (current_flag) {
      case 'b':
        flags.number_non_blank = true;
        break;
      case 'e':
        flags.mark_endl = true;
        flags.print_non_printable = true;
        break;
      case 'v':
        flags.print_non_printable = true;
        break;
      case 'E':
        flags.mark_endl = true;
        break;
      case 'n':
        flags.number_all = true;
        break;
      case 's':
        flags.squeeze = true;
        break;
      case 't':
        flags.print_non_printable = true;
        flags.tab = true;
        break;
      case 'T':
        flags.tab = true;
    }
  }
  return flags;
}

void CatFile(FILE *file, Flags flags, const char *table[256], int *line_num) {
  int c = 0, last = '\n';
  bool squeeze = false;
  while ((c = getc(file)) != EOF) {
    if (last == '\n') {
      if (flags.squeeze && c == '\n') {
        if (squeeze) continue;
        squeeze = true;
      } else
        squeeze = false;
      if (flags.number_non_blank) {
        if (c != '\n') printf("%6d\t", ++*line_num);
      } else if (flags.number_all)
        printf("%6d\t", ++*line_num);
    }
    if (!*table[c])
      printf("%c", '\0');
    else
      printf("%s", table[c]);
    last = c;
  }
}

void Cat(int argc, char *argv[], Flags flags, const char *table[static 256]) {
  int line_num = 0;
  for (char **filename = argv + optind; filename < argv + argc; ++filename) {
    FILE *file = fopen(*filename, "rb");
    if (errno) {
      fprintf(stderr, "%s: ", argv[0]);
      perror(*filename);
      continue;
    }
    CatFile(file, flags, table, &line_num);
    fclose(file);
  }
}