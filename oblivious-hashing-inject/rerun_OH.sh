# sudo rm bubblebc.bc
# sudo rm bubble_out.bc
# sudo rm bubble_out.o
# sudo rm a.out
rm -rf *.bc *o *.out

# Compile hash snippets
cc -c /home/sip/sip-oblivious-hashing/oblivious-hashing-setup/src/OHHashFunctions.c 

clang-3.9 -emit-llvm /home/sip/sip-oblivious-hashing/bubble_sort_print.cpp -c -o bubblebc.bc

opt-3.9  -load /home/sip/sip-oblivious-hashing/oblivious-hashing-setup/build/lib/libOHSetup.so bubblebc.bc -numHashVars 5 -oh-setup-pass -o bubble_out.bc
# Another pass here if needed?
llc-3.9 -filetype=obj ./bubble_out.bc

cc bubble_out.o OHHashFunctions.o

./a.out 5
