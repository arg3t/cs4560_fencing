# Compiling

```
cmake -DLLVM_LIT=lit -DLLVM_DIR=/usr/lib64/cmake/llvm  -S . -B build
cmake --build build
cd build
make
```


# Running the Pass
```
opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso <ll_file>
```


# Running the tests

```
make test
make run-lit-tests
```
