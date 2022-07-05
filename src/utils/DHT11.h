#ifndef __SIMPLEDHT11__
#define __SIMPLEDHT11__

#include "sensors_control.h"
#include "shared_memory.h"


#include <stdint.h>

#define DHTLIB_OK               0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2
#define DHTLIB_INVALID_VALUE    -999

#define DHTLIB_DHT11_WAKEUP     20
#define DHTLIB_DHT_WAKEUP       1

#define DHTLIB_TIMEOUT          100
#define DHT11_Pin               0

#define DHT11_SHM_PATH          "/DHT11"
#define DHT11_SHM_PTR_PATH      "/DHT11_ptr"



class DHT11_Operator: public Sensor {
    private:
        int pin;
        uint8_t bits[5];
        int readSensor(int wakeupDelay);
    public:
        DHT11_Operator(int _pin);
        
        double humidity,temperature;
        int readDHT11Once(); 
        int readDHT11();
};

class DHT11: public Sensor {
    public:
        
        DHT11() {
            timeBetweenAcquisition = 1000;
        }
        
        void info();
        void writeMemory(int pin, int len);
        Rcpp::List readMemory(int n);
        void killProcess();
};

#endif // __SIMPLEDHT11__