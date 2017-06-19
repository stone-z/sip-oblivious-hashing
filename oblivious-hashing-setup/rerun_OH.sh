# sudo rm bubblebc.bc
# sudo rm bubble_out.bc
# sudo rm bubble_out.o
# sudo rm a.out

cd build
make
cd ..

# InputFilename="bubble_sort.cpp"

# Compile hash snippets
cc -c src/OHHashFunctions.cpp

clang-3.9 -emit-llvm ../bubble_sort_print.cpp -c -o bubblebc.bc
opt-3.9 -load /usr/local/lib/libInputDependency.so -load ./build/lib/libOHSetup.so bubblebc.bc -numHashVars 5 -oh-setup-pass -o bubble_out.bc
# Another pass here if needed?
llc-3.9 -filetype=obj ./bubble_out.bc

# cc bubble_out.o
cc -rdynamic bubble_out.o OHHashFunctions.o

./a.out 5
