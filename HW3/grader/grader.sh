#!/bin/bash

truth_file_name="${1}.img"
starting_file_name="${2}.img"
fixed_file_name="${2}_fixed.img"
identifier_file_name="./${3}.txt"
identifier=$(head -n 1 $identifier_file_name)
grader_exec="./grader"
rec_exec="./recext2fs"

cp $starting_file_name $fixed_file_name
make all
$rec_exec $fixed_file_name $identifier
$grader_exec $truth_file_name $starting_file_name $fixed_file_name
