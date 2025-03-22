#ifndef S21_GREP
#define S21_GREP

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  int regex_flag;  //-e, -i
  char *pattern;
  bool invert;                    //-v
  bool count;                     //-c
  bool files_match;               //-l
  bool number_line;               //-n
  bool no_filename;               //-h
  bool no_incorrect_file_errors;  //-s
  bool print_matched;             //-o
  bool error_flag;
} grep_info;

#define MAX_LINE_SIZE 100000
#define EXIT_STATUS_OK 0
#define EXIT_STATUS_ERROR 2
#define ERROR_INVALID_FILE 3
#define ERROR_REGCOMP_FAIL 4

grep_info GrepReadInfo(int argc, char **argv);
bool ParseRegexFile(char **argv, char *filename, char **patterns);
void ErrorHandler(char **argv, int error_code, char *filename);
void GrepFile(FILE *file, grep_info *info, regex_t *preg, char *filename,
              int files_amount);
bool GrepParse(int argc, char **argv, grep_info *info);

#endif