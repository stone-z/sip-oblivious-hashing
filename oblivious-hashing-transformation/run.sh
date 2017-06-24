#!/bin/bash

rm -rf *.bc *o *.out

# Compile hash snippets
cc -c /home/sip/sip-oblivious-hashing/oblivious-hashing-setup/src/OHHashFunctions.c

clang-3.9 -emit-llvm /home/sip/sip-oblivious-hashing/bubble_sort_print.c -c -o bubblebc.bc

opt-3.9  -load /home/sip/sip-oblivious-hashing/oblivious-hashing-setup/build/lib/libOHSetup.so bubblebc.bc -numHashVars 5 -oh-setup-pass -o bubble_out.bc

opt-3.9 -load  /usr/local/lib/libInputDependency.so -load lib/libOHTransformation.so -oh-transform-pass -numHashVars 5 -hf simpleSum -hf simpleSumthingElse bubble_out.bc > bubble_2_out.bc

llc-3.9 -filetype=obj ./bubble_2_out.bc

cc bubble_2_out.o OHHashFunctions.o -o result

./result 2

