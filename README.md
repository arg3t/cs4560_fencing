# Compiling

```
mkdir build
cd build
cmake -DLLVM_DIR=/usr/lib64/cmake/llvm ..
make
```

# Running
```
clang -fpass-plugin=`echo build/passes/*.so` 
```
