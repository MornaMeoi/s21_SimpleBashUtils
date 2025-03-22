#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int exit_status = EXIT_STATUS_OK;
  grep_info info = GrepReadInfo(argc, argv);
  if (info.error_flag || GrepParse(argc, argv, &info))
    exit_status = EXIT_STATUS_ERROR;
  free(info.pattern);
  return exit_status;
}

void AddExpression(char **pattern, char const *expr) {
  int size = strlen(*pattern), expr_size = strlen(expr);
  *pattern = (char *)realloc(*pattern, size + expr_size + 2);
  (*pattern)[size] = '|';
  memcpy(*pattern + size + 1, expr, expr_size + 1);
}

bool ParseRegexFile(char **argv, char *filename, char **pattern) {
  bool good_file = true;
  FILE *file = fopen(filename, "rb");
  if (file) {
    char line[MAX_LINE_SIZE];
    while (fgets(line, MAX_LINE_SIZE, file)) {
      int temp = strlen(line) - 1;
      if (line[temp] == '\n') line[temp] = '\0';
      AddExpression(pattern, line);
    }
    fclose(file);
  } else {
    ErrorHandler(argv, ERROR_INVALID_FILE, filename);
    good_file = false;
  }
  return good_file;
}

grep_info GrepReadInfo(int argc, char **argv) {
  int current_flag = 0;
  grep_info info = {0,     NULL,  false, false, false,
                    false, false, false, false, false};
  info.pattern = (char *)malloc(sizeof(char));
  info.pattern[0] = '\0';
  while (!info.error_flag &&
         (current_flag = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) !=
             -1)
    switch (current_flag) {
      case 'e':
        if (info.pattern[0] == '\0') info.regex_flag |= REG_EXTENDED;
        AddExpression(&(info.pattern), optarg);
        break;
      case 'i':
        info.regex_flag |= REG_ICASE;
        break;
      case 'v':
        info.invert = true;
        break;
      case 'c':
        info.count = true;
        break;
      case 'l':
        info.files_match = true;
        break;
      case 'n':
        info.number_line = true;
        break;
      case 'h':
        info.no_filename = true;
        break;
      case 's':
        info.no_incorrect_file_errors = true;
        break;
      case 'f':
        info.error_flag = !ParseRegexFile(argv, optarg, &(info.pattern));
        break;
      case 'o':
        info.print_matched = true;
        break;
      default:
        info.error_flag = true;
    }
  return info;
}

bool GrepParse(int argc, char **argv, grep_info *info) {
  bool error = false;
  int files_amount = 0;
  if (info->pattern[0] == '\0' && optind < argc)
    AddExpression(&(info->pattern), argv[optind++]);
  regex_t preg_storage;
  if (regcomp(&preg_storage, (info->pattern) + 1, info->regex_flag)) {
    ErrorHandler(argv, ERROR_REGCOMP_FAIL, NULL);
    error = true;
  }

  for (char **filename = argv + optind;
       !error && filename < argv + argc && files_amount < 2; ++filename)
    ++files_amount;
  for (char **filename = argv + optind; !error && filename < argv + argc;
       ++filename) {
    FILE *file = fopen(*filename, "rb");
    if (file) {
      GrepFile(file, info, &preg_storage, *filename, files_amount);
      fclose(file);
    } else if (!info->no_incorrect_file_errors)
      ErrorHandler(argv, ERROR_INVALID_FILE, *filename);
  }
  regfree(&preg_storage);
  return error;
}

void PrintPrefix(grep_info *info, int files_amount, char *filename,
                 int line_num) {
  if (files_amount > 1) printf("%s:", filename);
  if (info->number_line) printf("%d:", line_num);
}

void GrepFile(FILE *file, grep_info *info, regex_t *preg, char *filename,
              int files_amount) {
  char line[MAX_LINE_SIZE];
  regmatch_t match;
  int line_num = 0, count = 0;
  bool good = true;
  while (fgets(line, MAX_LINE_SIZE, file) && good) {
    bool pattern_found = (info->invert && regexec(preg, line, 1, &match, 0)) ||
                         (!info->invert && !regexec(preg, line, 1, &match, 0));
    ++line_num;
    if (pattern_found) {
      if (info->files_match) {
        printf("%s\n", filename);
        good = false;
      } else if (info->count)
        ++count;
      else if (info->print_matched && !info->invert)
        for (char *remaining = line;
             (info->invert && regexec(preg, remaining, 1, &match, 0)) ||
             (!info->invert && !regexec(preg, remaining, 1, &match, 0));
             remaining += match.rm_eo) {
          PrintPrefix(info, files_amount, filename, line_num);
          printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                 remaining + match.rm_so);
        }
      else if (!info->print_matched) {
        PrintPrefix(info, files_amount, filename, line_num);
        printf("%s", line);
      }
      if (line[strlen(line) - 1] != '\n' && !info->print_matched &&
          !info->files_match)
        printf("\n");
    }
  }
  if (info->count && good) {
    if (files_amount > 1) printf("%s:", filename);
    printf("%d\n", count);
  }
}

void ErrorHandler(char **argv, int error_code, char *filename) {
  switch (error_code) {
    case ERROR_INVALID_FILE:
      fprintf(stderr, "%s: %s: No such file or directory\n", argv[0], filename);
      break;
    case ERROR_REGCOMP_FAIL:
      fprintf(stderr, "Failed to compile regex.\n");
  }
}