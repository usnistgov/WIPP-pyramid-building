By default OSX is shipped with a "secure" version of clang that is stripped down of some features.
In particular, the experimental::filesystem lib is not shipped with it.
It needs to be installed manually (we choose to install it in /usr/local/opt)

Then to compile the project, we need to configure it with cmake and to pass it our non-system compiler.

    cmake -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ ..
    

