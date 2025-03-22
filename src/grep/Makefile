CC=gcc
CFLAGS=-Werror -Wall -Wextra

TEST_FLAGS=-ie
TEST_PATTERN=Test
TEST_FILE=test.txt test.txt

all: rebuild
rebuild: clean build
build: s21_grep
s21_grep: s21_grep.o
	$(CC) $(CFLAGS) s21_grep.o -o s21_grep
s21_grep.o:
	$(CC) $(CFLAGS) -c s21_grep.c -o s21_grep.o
test: rebuild
	grep $(TEST_FLAGS) $(TEST_PATTERN) $(TEST_FILE) > 1.txt
	./s21_grep $(TEST_FLAGS) $(TEST_PATTERN) $(TEST_FILE) > 2.txt
	diff 1.txt 2.txt
	rm -f 1.txt 2.txt
ultimate_test: rebuild
	bash grep.test.sh
	bash grep_leak.test.sh
clean:
	rm -f *.o
	rm -f s21_grep