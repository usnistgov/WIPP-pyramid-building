//
// Created by Gerardin, Antoine D. (Assoc) on 1/16/19.
//



#include <string>
#include <iostream>
#include <thread>


#include "experimental/filesystem"
#include "testBasePerf.h"



void task(std::string msg){
    auto fovReader = new FOVReader<uint16_t>();
    fovReader->readAndForgetFOV(msg.c_str());
    delete fovReader;
    std::cout << msg << std::endl;

}

void do_join(std::thread& t)
{
    if (t.joinable())
        t.join();
}

int main()
{
    std::string directory = "/home/gerardin/Documents/images/dataset5_big/images/";

    auto begin = std::chrono::high_resolution_clock::now();

    if(! filesystem::exists(filesystem::current_path() / "testPerfOutput")) {
        filesystem::create_directory(filesystem::current_path() / "testPerfOutput");
    }


    uint32_t counter =0;

//    auto directory = filesystem::current_path().string() + "/testPerfOutput/";

    std::queue<std::string> threads;

    for (const auto & entry : filesystem::directory_iterator(directory)) {
        std::cout << entry.path() << std::endl;
        std::string file = entry.path().string();
        std::cout << file << std::endl;

    // fovReader->readFOV(file.c_str());
        auto inputFile = directory + entry.path().filename().string();
     //   fovReader->readWriteFOV(file.c_str(), outputFile.string());
//        "Threads" + std::to_string(counter++)
//        file.c_str()
        threads.push(inputFile);
    }

    std::queue<std::thread> runningThreads;

    while(threads.size() > 0) {

        while (runningThreads.size() < 10) {
            std::string file = threads.front();
            threads.pop();
            std::thread thread(task, file);
            runningThreads.push(std::move(thread));
            do_join(thread);
            runningThreads.pop();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " mS" << std::endl;


    return 0;
}


int main2(){

    std::string directory = "/home/gerardin/Documents/images/dataset5_big/images/";

    auto begin = std::chrono::high_resolution_clock::now();

//    std::string directory = "/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset01/images/";
//    auto filename = "test01-tiled.tif";

    if(! filesystem::exists(filesystem::current_path() / "testPerfOutput")) {
        filesystem::create_directory(filesystem::current_path() / "testPerfOutput");
    }

    auto fovReader = new FOVReader<uint16_t>();
    uint32_t counter =0;

//    auto directory = filesystem::current_path().string() + "/testPerfOutput/";

    std::vector<std::thread> threads(5000);

    for (const auto & entry : filesystem::directory_iterator(directory)) {
        std::cout << entry.path() << std::endl;
        std::string file = entry.path().string();
        std::cout << file << std::endl;

        // fovReader->readFOV(file.c_str());
        auto outputFile = filesystem::current_path() / "testPerfOutput/" / entry.path().filename();
        //   fovReader->readWriteFOV(file.c_str(), outputFile.string());
//        "Threads" + std::to_string(counter++)
//        file.c_str()
        threads.push_back(std::thread(task, file ));
    }


//    for(std::thread tdsa : threads){
//    }

//    for (std::vector<int>::iterator it = threads.begin() ; it != threads.end(); ++it){
//
//    }

    std::for_each(threads.begin(),threads.end(),do_join);
    delete fovReader;

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " mS" << std::endl;


    return 0;

}