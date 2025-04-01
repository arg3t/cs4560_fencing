# Compiling

```
mkdir build
cd build
cmake -DLLVM_DIR=/usr/lib64/cmake/llvm ..
make
```

# Running
```
opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso -o lb_fenced.ll lb.ll
```
