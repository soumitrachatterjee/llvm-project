This directory adds a new option -fdump-auto-type-inference to Clang.

Build the plugin by running the following commands:
Inside the build directory: 

$ cmake -DCLANG_BUILD_EXAMPLES=ON /path/to/llvm-project/llvm
$ make
$ make install-clang

Inside the install/bin directory (to run the plugin on a test file):

$ ./clang --cc1 -load /path/to/build/lib/our_fdump_auto_inference.so -plugin -fdump-auto-type-inference test.cpp
