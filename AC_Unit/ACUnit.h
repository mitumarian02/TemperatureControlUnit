class ACUnit
{
private:
    enum class AC_STATE : int
    {
        OFF = 0,
        COOLING = 1,
        HEATING = 2
    };
    AC_STATE state;
    int id;
    double roomTemperature;
    char *nameOfMemory;
    double *desiredTemperature, *exteriorTemperature;
    boost::interprocess::interprocess_mutex *mtx;
    bool *powerOff;
    bool thermostatConnected;

public:
    ACUnit(const ACUnit &);
    ACUnit(int);
    void heat();
    void cool();
    void run();
    void printState();
    AC_STATE getState();
    int getId();
    void setState(AC_STATE);
    double getRoomTemperature();
    void setRoomTemperature(double);
    double *getDesiredTemperature();
    double *getExteriorTemperature();
    bool *getPowerOff();
    bool requestPowerOff();
    ~ACUnit();
};