CC=gcc
CFLAGS=-Werror -Wall -Wextra

TEST_FLAGS=-n
TEST_FILE=test_1_cat.txt

all: rebuild
rebuild: clean build
build: s21_cat
s21_cat: cat_flags.o s21_cat.o
	$(CC) $(CFLAGS) cat_flags.o s21_cat.o -o s21_cat
s21_cat.o:
	$(CC) $(CFLAGS) -c s21_cat.c -o s21_cat.o
cat_flags.o:
	$(CC) $(CFLAGS) -c cat_flags.c -o cat_flags.o
test: rebuild
	cat $(TEST_FILE) $(TEST_FLAGS) > 1.txt
	./s21_cat $(TEST_FILE) $(TEST_FLAGS)  > 2.txt
	diff 1.txt 2.txt
	rm 1.txt 2.txt
ultimate_test: rebuild
	bash test_func_cat.sh
clean:
	rm -f *.o
	rm -f s21_cat
	rm -f cat_result