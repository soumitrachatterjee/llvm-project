#!/bin/bash

# Save the entire Clang command in a variable
# ./compile.sh <location_so> <so_name> a.c b.c c.c

compile_command="./compile.sh $*"
pos=1

for arg in "${@}"; do
    if [ "$pos" -eq 1 ]
    then
        so_loc=$arg
        pos=$((pos+1)) 
    elif [ "$pos" -eq 2 ]
    then 
        so_name=$arg
        pos=$((pos+1)) 
    else
        file=$arg
        out_name="${file%.*}.out"
        # building initial IR Code
        clang $file -S -emit-llvm -o init_ir.ll
        # passing initial IR Code through our custom pass
        opt -enable-new-pm=0 -load $so_loc -$so_name -compile-command="$file,$compile_command" \
         init_ir.ll -S -o "intermediate_ir_${file%.*}.ll"
        # converting the modified IR Code to a binary
        clang "intermediate_ir_${file%.*}.ll" -o $out_name
        rm init_ir.ll
        pos=$((pos+1)) 
    fi
done