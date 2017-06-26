#!/bin/bash

source=$1
numHashVars=$2

projectRoot=/home/sip/sip-oblivious-hashing

echo "ProjRoot: $projectRoot/test"
echo

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

echo

 rm -rf *.bc *o *.out

echo

# Compile hash snippets
cc -c $projectRoot/oblivious-hashing-inject/src/OHHashFunctions.c

echo

 
clang-3.9 -emit-llvm $source -c -o sourcebc.bc

echo

echo
echo Running Transform Pass

opt-3.9 -load  /usr/local/lib/libInputDependency.so -load $projectRoot/oblivious-hashing-transformation/build/lib/libOHTransformation.so -oh-transform-pass -numHashVars $numHashVars -hf simpleSum -hf simpleSumthingElse sourcebc.bc > prepared.bc

echo Running Inject Pass

opt-3.9  -load $projectRoot/oblivious-hashing-inject/build/lib/libOHInject.so prepared.bc -numHashVars $numHashVars -oh-inject-pass -o transformed.bc

llc-3.9 -filetype=obj ./transformed.bc

cc transformed.o OHHashFunctions.o
