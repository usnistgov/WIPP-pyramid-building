## Running pyramid building benchmarks

See example : benchmarks/benchmark-1.sh

If error message
    ```I’m sorry, `getopt --test` failed in this environment.```

install of getopt

On OSX : brew install -y gnu-getopt && echo 'export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"' >> ~/.bash_profile

--------------

## Running benchmarks of the old java code

to build an executable jar from the project root directory:

    mvn package

then to run the benchmark ```tools/run_benchmarks_java```.

A full example is in ```benchmarks/benchmark-7-java.sh```


gnu-getopt is required to run the bash script.

    brew install gnu-getopt

==> Caveats
gnu-getopt is keg-only, which means it was not symlinked into /usr/local,
because macOS already provides this software and installing another version in
parallel can cause all kinds of trouble.

If you need to have gnu-getopt first in your PATH run:
    
    echo 'export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"' >> ~/.bash_profile
