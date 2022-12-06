#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "ACUnit.cpp"
#include <iostream>

using namespace boost::interprocess;

int main()
{
    // shared memory of all ac's and thermometers
    shared_memory_object::remove("sharedMemory");
    managed_shared_memory managed_shm{create_only, "sharedMemory", 1024};
    int *noOfInstances = managed_shm.construct<int>("noOfInstances")(0);
    std::vector<ACUnit *> ACvector;
    std::vector<boost::thread> threadVector;
    int id, oldId = -1;
    do
    {
        id = *noOfInstances;
        if (id > oldId)
        {
            ACvector.push_back(new ACUnit(id));
            threadVector.push_back(boost::thread(boost::bind(&ACUnit::run, ACvector.at(id))));
            threadVector.push_back(boost::thread(boost::bind(&ACUnit::printState, ACvector.at(id))));
            std::cout << "****** Thread " << id << " created ******* \n";
        }
        oldId = id;
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    } while (*noOfInstances >= 0);

    for (ACUnit *AC : ACvector)
    {
        AC->requestPowerOff();
    }

    // join all threads created
    for (boost::thread &th : threadVector)
    {
        th.join();
    }
    // delete sharedMemory in case thermostat is started before ACUnit
    shared_memory_object::remove("sharedMemory");
}