#include <Rcpp.h>
#include <wiringPi.h>
#include <stdint.h>
#include <time.h>

#include "DHT11.h"

DHT::DHT() {
    wiringPiSetup();
}

int DHT::readDHTSensor(int pin, int wakeupDelay) {
    int mask = 0x80;
    int idx = 0;
    int32_t t,loopCnt;
    int chk;
    // 5 bytes of data
    for (int i = 0; i < 5;i++)
        bits[i] = 0;
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);
    delay(500);
    // start signal
    digitalWrite(pin,LOW);
    delay(wakeupDelay);
    digitalWrite(pin,HIGH);
    pinMode(pin,INPUT);

    loopCnt = SENSOR_TIMEOUT;
    t = micros();

    while (1) {
        if (digitalRead(pin) == LOW) break;
        if ((micros()-t)>loopCnt) return SENSOR_ERROR_TIMEOUT;
    }

    ////
    //Tell the Sensor to wait
    loopCnt = SENSOR_TIMEOUT;
    t = micros();
    // Wait for echo low level to end
    while (digitalRead(pin) == LOW) {
        if ((micros()-t)>loopCnt) return SENSOR_ERROR_TIMEOUT;
    }

    loopCnt = SENSOR_TIMEOUT;
    t = micros();
    // Wait for echo high level to end
    while (digitalRead(pin) == HIGH) {
        if ((micros()-t)>loopCnt) return SENSOR_ERROR_TIMEOUT;
    }
    //End of Wait
    ////

    for (int i = 0; i < 40; i++) {

        ////
        //Tell the Sensor to wait
        loopCnt = SENSOR_TIMEOUT;
        t = micros();
        // Wait for echo low level to end
        while (digitalRead(pin) == LOW) {
            if ((micros()-t)>loopCnt) return SENSOR_ERROR_TIMEOUT;
        }

        loopCnt = SENSOR_TIMEOUT;
        t = micros();
        // Wait for echo high level to end
        while (digitalRead(pin) == HIGH) {
            if ((micros()-t)>loopCnt) return SENSOR_ERROR_TIMEOUT;
        }
        //End of Wait
        ////

        // If the signal is pulled down
        if ((micros() - t) > 60) {
            bits[idx] |= mask;
        }

        mask >>= 1;
        if (mask == 0) {
            mask = 0x80;
            idx++;
        }

    }
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);
    return SENSOR_OK;
}

int DHT::readDHT11Once(int pin){
    int rv ;
    uint8_t checksum;
    rv = readDHTSensor(pin,DHTLIB_DHT11_WAKEUP);
    if(rv != SENSOR_OK){
        humidity = DHTLIB_INVALID_VALUE;
        temperature = DHTLIB_INVALID_VALUE;
        return rv;
    }
    humidity = bits[0];
    temperature = bits[2] + bits[3] * 0.1;
    checksum = bits[0] + bits[1] + bits[2] + bits[3];
    if(bits[4] != checksum)
        return DHTLIB_ERROR_CHECKSUM;
    return SENSOR_OK;
}
int DHT::readDHT11(int pin){
	int chk = DHTLIB_INVALID_VALUE;
	for (int i = 0; i < 15; i++){
		chk = readDHT11Once(pin);	//read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
		if(chk == SENSOR_OK){
			return SENSOR_OK;
		}
		delay(100);
	}
	return chk;
}

int DHT::readPhoRSensor(int pin) {
    pinMode(pin,OUTPUT);
    digitalWrite(pin,LOW);
    delay(100);

    pinMode(pin,INPUT);
    clock_t t= clock(), diff=0;
    
    while (digitalRead(pin) == LOW) {
        diff = clock() - t;
        // if (diff > PhoRSENSOR_TIMEOUT) {
        //     return SENSOR_ERROR_TIMEOUT;
        // }
    }
    PhoR_time_to_charge = (double) diff/ CLOCKS_PER_SEC;
    return SENSOR_OK;
}