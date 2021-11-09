#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H


#include <Rcpp.h>
#include <wiringPi.h>
#include <time.h> //for time
#include <unistd.h>
#include <chrono> // for measuring real execution time
#include <thread> // for sleep
#include <R.h>
#include <Rdefines.h> //R_ToplevelExec()
#include <R_ext/Utils.h> // R_CheckUserInterrupt()


#define DATE_STRING_SIZE 30

void check_interrupt_fn(void *dummy);
int pending_interrupt();

class Sensor {
public:
    
    time_t rawtime;
    int timeBetweenAcquisition = 1000;
    time_t get_raw_time();
    std::string to_time_string(time_t rawtime);

    
    void setTime(int _timeBetweenAcquisition);
    // virtual void info() = 0;
    // virtual void writeMemory(int pin) = 0;
    // virtual Rcpp::List readMemory(int n) = 0;
    // void freeMemory();
    // virtual void killProcess() = 0;
};

#endif // SENSOR_CONTROL_H