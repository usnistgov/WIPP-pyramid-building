By default OSX is shipped with a "secure" version of clang that is stripped down of some features.
In particular, the experimental::filesystem lib is not shipped with it.
A full version of the compiler must be installed manually (we choose to install it in /usr/local/opt)

Then to compile the project, we need to configure cmake and to use our non-system compiler.

    cmake -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ ..
    

