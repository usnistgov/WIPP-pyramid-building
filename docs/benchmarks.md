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

