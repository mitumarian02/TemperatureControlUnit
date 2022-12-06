#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/thread.hpp>
#include "thermostat.h"
#include <iostream>
#pragma once
using namespace boost::interprocess;

#define MIN_EXTERIOR_TEMPERATURE -10
#define MAX_EXTERIOR_TEMPERATURE 40
#define MIN_DESIRED_TEMPERATURE 16
#define MAX_DESIRED_TEMPERATURE 34

void Thermostat::readTemperatures()
{
    bool correctTemperature{false};
    int temperature;
    std::string choice{};
    while (choice != "1" && choice != "2" && choice != "exit")
    {
        std::cout << "What do you want to do?\n1 - Enter desired temperature\n2 - Enter exterior temperature\nexit - Exit the program\n";
        std::cin >> choice;
    }

    if (choice == "exit")
    {
        *powerOffShared = true; // power off the ACUnit
        state = false;          // power off the thermometer
        std::cout << "Exiting the program...\n";
        return;
    }

    while (!correctTemperature)
    {
        switch (stoi(choice))
        {
        case 1:
            std::cout << "Enter desired temperature: ";
            std::cin >> temperature;
            if (temperature < MIN_DESIRED_TEMPERATURE || temperature > MAX_DESIRED_TEMPERATURE)
            {
                correctTemperature = false;
                std::cout << "Temperature exceeds limits!\n";
            }
            else
            {
                correctTemperature = true;
            }
            break;
        case 2:
            std::cout << "Enter exterior temperature: ";
            std::cin >> temperature;
            if (temperature < MIN_EXTERIOR_TEMPERATURE || temperature > MAX_EXTERIOR_TEMPERATURE)
            {
                correctTemperature = false;
                std::cout << "Temperature exceeds limits!\n";
            }
            else
            {
                correctTemperature = true;
            }
            break;
        default:
            break;
        }
    }
    switch (stoi(choice))
    {
    case 1:
        this->desiredTemperature = temperature;
        std::cout << "Desired temperature read successfully!\n";
        break;
    case 2:
        this->exteriorTemperature = temperature;
        std::cout << "Exterior temperature read successfully!\n";
        break;
    default:
        break;
    }
    std::cout << "\n";
}

void Thermostat::sendTemperatures()
{
    this->mtx->lock();
    *this->desiredTemperatureShared = desiredTemperature;
    *this->exteriorTemperatureShared = exteriorTemperature;
    this->mtx->unlock();
}

Thermostat::Thermostat(int id)
{
    // construct name of shared memory based on id (ex:tempreratureMemory5, temperatureMemory16)
    std::string idString = std::to_string(id);
    std::string nameOfMemoryString = "temperatureMemory" + idString;
    int length = nameOfMemoryString.length();
    char nameOfMemory[length + 1];
    strcpy(nameOfMemory, nameOfMemoryString.c_str());
    // open memory and set initial values
    this->managed_shm = new managed_shared_memory{boost::interprocess::open_only, nameOfMemory};
    this->powerOffShared = managed_shm->find<bool>("powerOff").first;
    this->desiredTemperatureShared = managed_shm->find<double>("desiredTemperature").first;
    this->exteriorTemperatureShared = managed_shm->find<double>("exteriorTemperature").first;
    this->mtx = managed_shm->find<interprocess_mutex>("mutex").first;
    this->desiredTemperature = -99;
    this->exteriorTemperature = -99;
    this->state = true;
}

bool Thermostat::getState()
{
    return state;
}