#ifndef MQ2_H
#define MQ2_H

#include "sensors_control.h"
#include "shared_memory.h"


#define MQ2_SENSOR_OK              0

#define MQ2_SHM_PATH           "/MQ2"
#define MQ2_SHM_PTR_PATH       "/MQ2_ptr"


class MQ2_Operator : public Sensor {
    private:
        int SPICLK;
        int SPIMISO;
        int SPIMOSI;
        int SPICS;
        int mq2_dpin;
        int mq2_apin;
    public:
        MQ2_Operator(int _SPICLK, int _SPIMISO, int _SPIMOSI, int _SPICS, int _mq2_dpin, int _mq2_apin);
        double MQ2_time_to_charge, leak;
        int readMQ2();
        
};

class MQ2: public Sensor {
    public:
        MQ2(){
            timeBetweenAcquisition = 1000;
        }
        void info();
        void writeMemory(int _SPICLK, int _SPIMISO, int _SPIMOSI, int _SPICS, int _mq2_dpin, int _mq2_apin, int len);
        Rcpp::List readMemory(int n);
        void killProcess();
};

#endif // MQ2_H
