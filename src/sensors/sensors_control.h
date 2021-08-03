#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <Rcpp.h>




class Sensor {
public:
    virtual void writeMemory(int pin) = 0;
    virtual Rcpp::List readMemory() = 0;
    virtual void freememory() = 0;
    virtual void killProcess() = 0;
}

#endif // SENSOR_CONTROL_H