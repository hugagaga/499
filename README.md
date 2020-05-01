# CSCI 499 Warble Project

## **Introduction**
A Twitter-like command line C++ application built on the Faas Platform that communicates using gRPC.

## **Setup**
### **Virtual Environment**
#### Install [Virtualbox 5.2.36](https://download.virtualbox.org/virtualbox/5.2.36/VirtualBox-5.2.36-135684-Win.exe)
#### Install [Vagrant 2.2.7](https://www.vagrantup.com/downloads.html)  
    1. Create a file to store vagrant environment
    2. Open terminal within the file created before and set up Vagrant  
        vagrant init ubuntu/bionic64  
        vagrant up
    3. Access Vagrant virtual environment
        vagrant ssh
### **Library Installation**
#### Install [gRPC](https://github.com/grpc/grpc/) and submodules  
##### Install pkg-config:  
    sudo apt-get update  
    sudo apt-get install pkg-config
##### Install dependencies: autoconf,automake, libtool, make, g++,unzip, libgflags-dev, libgtest-dev, clang, libc++-dev:  
    sudo apt-get install autoconf automake libtool make g++ unzip  
    sudo apt-get install libgflags-dev libgtest-dev  
    sudo apt-get install clang libc++-dev
##### Download gRPC Source code and update submodules:  
    git clone https://github.com/grpc/grpc.git  
    cd grpc  
    git submodule update --init
##### Install protobuf (currently under directory /grpc)
    // Update submodules    
    cd third_party/protobuf/  
    git submodule update --init --recursive  
    // Generate configure file  
    sudo ./autogen.sh
    // Generate Makefile (default installation directory: usr/local/bin/  
    sudo ./configure
    // Compile         
    sudo make
    // Install                 
    sudo make install 
    // Update shared library cache
    sudo ldconfig
    // Check installation directory
    which protoc
    // Check if protobuf is successfully installed       
    protoc --version
#####  Install gRPC  
    // Back to /grpc root directory  
    cd ../..
    // Compile
    sudo make
    // Install
    sudo make install
#####  Test gRPC Example: Hello world!
    cd examples/cpp/helloworld/  
    make  
    ./greeter_server // Shows "Server Listening on 0.0.0.0:50051   
    // On another terminal connect to virtual environment using "vagrant ssh" or using tmux
    ./greeter_client // Shows "Greeter received: Hello world

#### Install glog
    sudo apt-get install libgoogle-glog-dev  
#### Install gtest
    sudo apt-get install libgtest-dev
    // Install CMake
    sudo apt-get install cmake 
    // Compile
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    // copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
    sudo cp *.a /usr/lib
## **Usage**
### Clone repository
### Build code  
    cd /src
    make
### Run code
1. Start back-end Key-Value Store Server   
```
./kvstore/kvstore_server
```
2. Start Faas platform Fuuc Server 
``` 
./func/func_server
```  
3. Run Warble command line tool  
```
./warble/warble
```
#### Warble Command Line Flags Usage  
(The order of flags does not matter.)
* --registeruser (username)  
Register a user by a unique username
* --warble (text) --user (username)   
Returns posted warble information including
    * Username
    * Wable id
    * Time
    * Content
* --warble (text) --user (username) --reply (warble id)  
Returns posted warble information including
    * Username
    * Wable id
    * Parent warble id
    * Time
    * Content
* --follow (username) --user (username)  
Follow a user by username
* --read (warble id) --user (username)  
Read a thread of warbles start with a warble of (warble id)
* --profile --user (username)  
Shows a user's following and followers by username
* --stream (hashtag) --user (username)  
Shows all warbles posted that contain #hashtag after stream() call.
### Test code
* Run basic test cases of back-end kvstore
```
./kvstore/kvmap_test
```
