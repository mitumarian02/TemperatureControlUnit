class Thermostat
{
private:
    double desiredTemperature;
    double exteriorTemperature;
    bool state;
    bool *powerOffShared;
    double *desiredTemperatureShared;
    double *exteriorTemperatureShared;
    boost::interprocess::managed_shared_memory *managed_shm;
    boost::interprocess::interprocess_mutex *mtx;

public:
    Thermostat(int);
    void readTemperatures();
    void sendTemperatures();
    bool getState();
};