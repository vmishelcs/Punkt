# Building the project

## Installing LLVM 18.1.2

In a directory other than the Punkt project directory, download the [LLVM project](https://github.com/llvm/llvm-project) release 18.1.2.

```
wget https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-18.1.2.tar.gz
```

Extract the LLVM project.

```
tar -xvzf llvmorg-18.1.2.tar.gz
```

Navigate to the `llvm-project-llvmorg-18.1.2` directory and configure the LLVM project. I use the `-DLLVM_PARALLEL_LINK_JOBS=4` build option because my memory gets completely eaten up by the linker for some reason. You may want to increase or decrease this number.

```
cmake -S llvm -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_PARALLEL_LINK_JOBS=4
```

Install LLVM.

```
sudo ninja -C build install
```

## Installing Clang

This project compiles code to LLVM IR, so we will need Clang to run the target code. I used `clang-15` to work on this project, however any version that supports opaque pointers (15 and later) should work.

```
sudo apt install clang-15
```
