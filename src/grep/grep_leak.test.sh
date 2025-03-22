#!/bin/bash

TEST_TEMPLATE="__test__/test_template.txt"
TEST_TMP1="tmp1"
Red='\033[0;31m'
Green='\033[0;32m'
Purple='\033[0;35m'
NC='\033[0m'

make all

SUCCESS_COUNT=0
FAIL_COUNT=0

function fn {
	leaks -quiet -atExit -- ./s21_grep -e ret -f $TEST_TEMPLATE $1 $2 > $TEST_TMP1
    leak=$(grep -A100000 leaks $TEST_TMP1)
	if [[ $leak == *"0 leaks for 0 total leaked bytes"* ]];
	then
		echo "${Purple}Command: leaks -quiet -atExit -- ./s21_grep -e ret -f $TEST_TEMPLATE $1 $2${NC}"
		echo "${Green}SUCCESS${NC}";
		SUCCESS_COUNT=$((SUCCESS_COUNT + 1))
	else
		echo "${Purple}Command: leaks -quiet -atExit -- ./s21_grep grep -e ret -f $TEST_TEMPLATE $1 $2${NC}"
		echo "${Red}FAIL${NC}";
		FAIL_COUNT=$((FAIL_COUNT + 1))
	fi
	rm $TEST_TMP1
}

# option testing array
options=("-i" "-v" "-c" "-l" "-n" "-h" "-s" "-o" "-iv" "-in")
files=("Makefile" "__test__/test_0_grep.txt" "__test__/test_1_grep.txt" "__test__/test_2_grep.txt" "__test__/test_3_grep.txt" "__test__/test_4_grep.txt" "__test__/test_5_grep.txt" "__test__/test_6_grep.txt")

for ix in ${!options[@]}
do
	for x in ${!files[@]}
	do
  		fn ${options[$ix]} ${files[$x]}
	done
done;

make clean

echo "${Green}Success: $SUCCESS_COUNT${NC}"
echo "${Red}Fail: $FAIL_COUNT${NC}"