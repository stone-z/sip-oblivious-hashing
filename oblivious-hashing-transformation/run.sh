#!/bin/bash

source=$1
numHashVars=$2

if [ ! -f $source ]
then
   echo "$0: usage: ./run.sh <csource> <num_hash_vars>"
   echo "Path is not valid"
   exit 1
fi

if [ $numHashVars -lt 1 -o $numHashVars -gt 99 ]
then
   echo "$0: usage: ./run.sh <csource> <num_hash_vars>"
   echo "Number of hash variables should be between 1 and 99"
   exit 1
fi

rm -rf *.bc *o *.out

# Compile hash snippets
cc -c /home/sip/sip-oblivious-hashing/oblivious-hashing-setup/src/OHHashFunctions.c

clang-3.9 -emit-llvm $source -c -o inputbc.bc

opt-3.9  -load /home/sip/sip-oblivious-hashing/oblivious-hashing-setup/build/lib/libOHSetup.so inputbc.bc -numHashVars 5 -oh-setup-pass -o input_out.bc

opt-3.9 -load  /usr/local/lib/libInputDependency.so -load lib/libOHTransformation.so -oh-transform-pass -numHashVars 5 -hf simpleSum -hf simpleSumthingElse input_out.bc > input_2_out.bc

llc-3.9 -filetype=obj ./input_2_out.bc

cc input_2_out.o OHHashFunctions.o -o result

./result 2

