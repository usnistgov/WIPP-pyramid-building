# Packaging and distribution


## Building the pyramid building docker container.

Packaging and distribution is provided through containerization.
The dist/ folder contains a Dockerfile describing the container install.

In order to achieve a small container size, some dependencies are provided
by linux package and linked to dynamically by the executable, while other
are statically linked.

In order to build the container, you need to first build the executable locally on a linux machine 
(check install.md for the list of dependencies that needs to be installed).

    cd dist/
    mdkir build && cd build/
    cmake ../..
    make 
  
The dist/build folder should now contain a ```main``` executable.
  
To build an image :
  
    docker build -t pyramid-building:1.0.0 .
    
    
To test the container : 

Edit ```run.sh``` with the paths to your test data and run the script.

Environment variables, docker parameters and program arguments help you tailor the container execution to the user needs. 


## WIPP plugin integration.

```dist/plugin-descriptor.json``` can be used to interface this container with the wipp plugin framework.