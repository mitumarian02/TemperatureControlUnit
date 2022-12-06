#include "thermostat.cpp"
#include <iostream>

bool memoryExists();

int main()
{
    // wait untill temperatureMemory adn sharedMemory is created
    while (!memoryExists())
    {
        // do nothing;
        ;
    }
    managed_shared_memory sharedMemory{open_only, "sharedMemory"};
    int *noOfInstances = sharedMemory.find<int>("noOfInstances").first;
    std::cout << "ID = " << *noOfInstances << "\n";
    int id = *noOfInstances;
    *noOfInstances = *noOfInstances + 1;
    Thermostat t(id);
    std::cout << "Thermostat started...\n";
    while (t.getState())
    {
        t.readTemperatures();
        t.sendTemperatures();
    }
    *noOfInstances = *noOfInstances - 1;

    // stop listening for a new thermostat if all thermostats are off
    if (*noOfInstances == 0)
    {
        *noOfInstances = -1;
    }
}

// check if "temperatureMemory" has been created
bool memoryExists()
{
    try
    {
        managed_shared_memory sharedMemory{open_only, "sharedMemory"};
        int *id = sharedMemory.find<int>("noOfInstances").first;
        std::string idString = std::to_string(*id);
        std::string nameOfMemoryString = "temperatureMemory" + idString;
        int length = nameOfMemoryString.length();
        char nameOfMemory[length + 1];
        strcpy(nameOfMemory, nameOfMemoryString.c_str());
        managed_shared_memory segment(open_only, nameOfMemory);
        return segment.check_sanity();
    }
    catch (const std::exception &ex)
    {
        std::cout << "AC Unit not started...\n";
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }
    return false;
}