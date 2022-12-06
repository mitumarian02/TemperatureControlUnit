#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/thread.hpp>
#include "ACUnit.h"
#include <iostream>
#include <condition_variable>
#pragma once
using namespace boost::interprocess;

void ACUnit::cool()
{
    while (*desiredTemperature - roomTemperature <= -0.5)
    {
        roomTemperature -= 0.5;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    }
    state = AC_STATE::OFF;
}

void ACUnit::heat()
{
    while (*desiredTemperature - roomTemperature >= 0.5)
    {
        roomTemperature += 0.5;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    }
    state = AC_STATE::OFF;
}

void ACUnit::printState()
{
    while (!thermostatConnected && *this->powerOff == false)
    {
        std::cout << "\nThermostat " << id << " not connected...\n";
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }

    // Print information about temperature and state every second
    std::cout << "\n";
    while (1)
    {
        // Exit the program if "exit" is executed in Thermostat
        if (*powerOff == true)
        {
            std::cout << "Power OFF \n";
            return;
        }

        std::cout << "Room - " << id << "\n";

        switch (state)
        {
        case AC_STATE::OFF:
            std::cout << "AC is off\n";
            break;
        case AC_STATE::COOLING:
            std::cout << "Cooling..\n";
            break;
        case AC_STATE::HEATING:
            std::cout << "Heating...\n";
            break;
        default:
            break;
        }
        this->mtx->lock();
        std::cout << "Desired temperature: " << *desiredTemperature << "\n";
        std::cout << "Room temperature: " << roomTemperature << "\n";
        std::cout << "Exterior temperature: " << *exteriorTemperature << "\n";
        this->mtx->unlock();
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
        std::cout << "\n";
    }
}

void ACUnit::run()
{
    while ((*this->desiredTemperature == -99 || *this->exteriorTemperature == -99) && *this->powerOff == false)
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }

    thermostatConnected = true;

    while (1)
    {
        // Exit the program if "exit" is executed in Thermostat
        if (*powerOff)
        {
            break;
        }

        switch (state)
        {
        case AC_STATE::OFF:
        {
            // increase temperature with 0.1 degrees every second if exterior temperature is higher than room temperature and AC is OFF
            if (*exteriorTemperature > roomTemperature)
            {
                roomTemperature = roomTemperature + 0.1;
                boost::this_thread::sleep_for(boost::chrono::seconds(1));
            }
            else if (*exteriorTemperature < roomTemperature)
            {
                roomTemperature = roomTemperature - 0.1;
                boost::this_thread::sleep_for(boost::chrono::seconds(1));
            }

            // switch to COOL case if room temperature is higher than desired temperature
            if (*desiredTemperature - roomTemperature < -1)
            {
                state = AC_STATE::COOLING;
            }

            // switch to HEAT case if room temperature is higher than desired temperature
            if (*desiredTemperature - roomTemperature > 1)
            {
                state = AC_STATE::HEATING;
            }
        }
        break;
        case AC_STATE::COOLING:
        {
            cool();
        }
        break;
        case AC_STATE::HEATING:
        {
            heat();
        }
        break;
        default:
            break;
        }
    }
}

int ACUnit::getId()
{
    return id;
}
ACUnit::AC_STATE ACUnit::getState()
{
    return state;
}

double *ACUnit::getDesiredTemperature()
{
    return this->desiredTemperature;
}

double *ACUnit::getExteriorTemperature()
{
    return this->exteriorTemperature;
}

bool *ACUnit::getPowerOff()
{
    return powerOff;
}

double ACUnit::getRoomTemperature()
{
    return roomTemperature;
}

void ACUnit::setRoomTemperature(double newTemperature)
{
    roomTemperature = newTemperature;
}

void ACUnit::setState(AC_STATE newState)
{
    state = newState;
}

bool ACUnit::requestPowerOff()
{
    *this->powerOff = true;
    if (*this->powerOff)
    {
        return true;
    }
    return false;
}

ACUnit::ACUnit(int id)
{
    this->id = id;
    // construct name of shared temperature memory
    std::string idString = std::to_string(id);
    std::string nameOfMemoryString = "temperatureMemory" + idString;
    int length = nameOfMemoryString.length();
    char nameOfMemoryLocal[length + 1];
    strcpy(nameOfMemoryLocal, nameOfMemoryString.c_str());
    this->nameOfMemory = new char[length + 1];
    strcpy(this->nameOfMemory, nameOfMemoryLocal);

    // alocate shared memory
    shared_memory_object::remove(nameOfMemory);
    managed_shared_memory *managed_shm = new managed_shared_memory{create_only, nameOfMemory, 1024};
    this->desiredTemperature = managed_shm->construct<double>("desiredTemperature")(-99);
    this->exteriorTemperature = managed_shm->construct<double>("exteriorTemperature")(-99);
    this->powerOff = managed_shm->construct<bool>("powerOff")(false);
    this->mtx = managed_shm->construct<interprocess_mutex>("mutex")();
    this->roomTemperature = 20;
    this->state = AC_STATE::OFF;
    thermostatConnected = false;
    std::cout << "Alocated " << nameOfMemory << "\n";
}

ACUnit::ACUnit(const ACUnit &unitCopy)
{
    this->id = unitCopy.id;
    this->state = unitCopy.state;
    this->roomTemperature = unitCopy.roomTemperature;
    this->desiredTemperature = unitCopy.desiredTemperature;
    this->exteriorTemperature = unitCopy.exteriorTemperature;
    this->powerOff = unitCopy.powerOff;
}

ACUnit::~ACUnit()
{
    delete nameOfMemory, desiredTemperature, exteriorTemperature;
    delete mtx;
    delete powerOff;
}