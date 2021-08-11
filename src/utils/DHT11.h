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
        uint8_t bits[5];
        int readSensor(int pin, int wakeupDelay);
    public:
        DHT11_Operator();
        
        double humidity,temperature;
        int readDHT11Once(int pin); 
        int readDHT11(int pin);
};

class DHT11: public Sensor, public SharedMemory {
    public:
        
        
        DHT11(const char* dht11_shmpath,const char* dht11_shmpath_ptr ):SharedMemory(dht11_shmpath,dht11_shmpath_ptr) {
            timeBetweenAcquisition = 1000;
            DataBlock db1(BLOCK_LENGTH,BLOCK_LENGTH,BLOCK_LENGTH,1,1);
            DataPtr dp1(BLOCK_LENGTH);
            data_obj = &db1;
            ptr_obj = &dp1;
        }
        DataBlock* data_obj;
        DataPtr* ptr_obj;
        void info();
        void writeMemory(int pin);
        Rcpp::List readMemory(int n);
        void killProcess();
};

#endif // __SIMPLEDHT11__