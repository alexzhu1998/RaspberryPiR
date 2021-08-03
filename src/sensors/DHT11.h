#ifndef __SIMPLEDHT11__
#define __SIMPLEDHT11__

#include "sensors_control.h"

#include <wiringPi.h>
#include <stdint.h>

#define DHTLIB_OK               0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2
#define DHTLIB_INVALID_VALUE    -999

#define DHTLIB_DHT11_WAKEUP     20
#define DHTLIB_DHT_WAKEUP       1

#define DHTLIB_TIMEOUT          100
#define DHT11_Pin  0


class DHT11: public Sensor {
    public:
        DHT11();
        double humidity,temperature;
        int readDHT11Once(int pin); 
        int readDHT11(int pin);
    private:
        uint8_t bits[5];
        int readSensor(int pin, int wakeupDelay);
};

#endif // __SIMPLEDHT11__