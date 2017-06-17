rm bubblebc.bc
cd build
make
cd ..

clang-3.9 -emit-llvm bubble_sort.cpp -c -o bubblebc.bc
opt-3.9 -load /usr/local/lib/libInputDependency.so -load ./build/lib/libskeleton.so bubblebc.bc -input-dep-skeleton -o bubble_out.bc

