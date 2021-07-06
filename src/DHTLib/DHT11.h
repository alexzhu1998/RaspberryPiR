#ifndef __SIMPLEDHT11__
#define __SIMPLEDHT11__


#include <wiringPi.h>
#include <stdint.h>

#define SENSOR_OK               0
#define DHTLIB_ERROR_CHECKSUM   -1
#define SENSOR_ERROR_TIMEOUT    -2
#define DHTLIB_INVALID_VALUE    -999

#define DHTLIB_DHT11_WAKEUP     20
#define DHTLIB_DHT_WAKEUP       1

#define SENSOR_TIMEOUT          100
#define PhoRSENSOR_TIMEOUT      50000
#define DHT11_Pin  0


class DHT {
    public:
        DHT();
        double humidity,temperature;
        double PhoR_time_to_charge;
        
        int readDHT11(int pin);
        int readPhoRSensor(int pin);
    private:
        uint8_t bits[5];
        int readDHTSensor(int pin, int wakeupDelay);
        int readDHT11Once(int pin); 
        
};


#endif // __SIMPLEDHT11__