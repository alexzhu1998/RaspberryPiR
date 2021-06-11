// #include "Fibonacci.h"
extern "C" {
#include <wiringPi.h>
}
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include <stdio.h>
#include <R.h>
#include <Rdefines.h>
// #include <Rcpp.h>


// using namespace Rcpp;

#define DHTLIB_OK               0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2
#define DHTLIB_INVALID_VALUE    -999

#define DHTLIB_DHT11_WAKEUP     20
#define DHTLIB_DHT_WAKEUP       1

#define DHTLIB_TIMEOUT          100
#define DHT11_Pin  0

#define DELAY 1000
#define STARTUP_DELAY 500
#define FIXED_ERROR_DELAY 100

// DHT11 has a sampling rate of 1Hz according to https://learn.adafruit.com/dht?view=all

class DHT{
public:
    DHT();
    double humidity,temperature;    //use to store temperature and humidity data read
    int readDHT11Once(int pin, unsigned int readDelay);     //read DHT11
    int readDHT11(int pin, unsigned int readDelay);     //read DHT11
private:
    u_int8_t bits[5];    //Buffer to receiver data
    int readSensor(int pin,int wakeupDelay, unsigned int readDelay);    //

};


DHT::DHT(){
    wiringPiSetup();
}

int DHT::readSensor(int pin,int wakeupDelay,unsigned int readDelay){
    int mask = 0x80;
    int idx = 0;
    int i;
    int32_t t;
    for (i=0;i<5;i++){
        bits[i] = 0;
    }
    // Clear sda
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);
    delay(readDelay);
    // Start signal
    digitalWrite(pin,LOW);
    delay(wakeupDelay);
    digitalWrite(pin,HIGH);
    // delayMicroseconds(40);
    pinMode(pin,INPUT);

    // printf("C++_Initialisation_Complete\n");
    int32_t loopCnt = DHTLIB_TIMEOUT;
    t = micros();
    // Waiting echo
    while(1){
        if(digitalRead(pin)==LOW){
            break;
        }
        if((micros() - t) > loopCnt){
            return DHTLIB_ERROR_TIMEOUT;
        }
    }

    loopCnt = DHTLIB_TIMEOUT;
    t = micros();
    // Waiting echo low level end
    while(digitalRead(pin)==LOW){
        if((micros() - t) > loopCnt){
            return DHTLIB_ERROR_TIMEOUT;
        }
    }

    loopCnt = DHTLIB_TIMEOUT;
    t = micros();
    // Waiting echo high level end
    while(digitalRead(pin)==HIGH){
        if((micros() - t) > loopCnt){
            return DHTLIB_ERROR_TIMEOUT;
        }
    }
    for (i = 0; i<40;i++){
        loopCnt = DHTLIB_TIMEOUT;
        t = micros();
        while(digitalRead(pin)==LOW){
            if((micros() - t) > loopCnt)
                return DHTLIB_ERROR_TIMEOUT;
        }
        t = micros();
        loopCnt = DHTLIB_TIMEOUT;
        while(digitalRead(pin)==HIGH){
            if((micros() - t) > loopCnt)
                return DHTLIB_ERROR_TIMEOUT;

        }
        if((micros() - t ) > 60){
            bits[idx] |= mask;
        }
        mask >>= 1;
        if(mask == 0){
            mask = 0x80;
            idx++;
        }
    }
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);
    //printf("bits:\t%d,\t%d,\t%d,\t%d,\t%d\n",bits[0],bits[1],bits[2],bits[3],bits[4]);
    return DHTLIB_OK;
}
//Function：Read DHT sensor, analyze the data of temperature and humidity
//return：DHTLIB_OK   DHTLIB_ERROR_CHECKSUM  DHTLIB_ERROR_TIMEOUT
int DHT::readDHT11Once(int pin, unsigned int readDelay){
    int rv ;
    u_int8_t sum;
    rv = readSensor(pin,DHTLIB_DHT11_WAKEUP, readDelay);
    if(rv != DHTLIB_OK){
        humidity = DHTLIB_INVALID_VALUE;
        temperature = DHTLIB_INVALID_VALUE;
        return rv;
    }
    humidity = bits[0];
    temperature = bits[2] + bits[3] * 0.1;
    sum = bits[0] + bits[1] + bits[2] + bits[3];
    if(bits[4] != sum) return DHTLIB_ERROR_CHECKSUM;
    return DHTLIB_OK;
}

int DHT::readDHT11(int pin, unsigned int readDelay){
    int chk = DHTLIB_INVALID_VALUE;
    for (int i = 0; i < 15; i++){
        chk = readDHT11Once(pin,readDelay);	//read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
        if(chk == DHTLIB_OK){
            return DHTLIB_OK;
        }
        // printf("%d",*readDelay);
        delay(FIXED_ERROR_DELAY);
    }
    return chk;
}

// RCPP_MODULE(foo) {
//     class_<DHT>("DHT")
//     .constructor()
//     .method("readDHT11",&DHT::readDHT11)
//     .method("readDHT11Once",&DHT::readDHT11Once)
//     .method("readSensor",&DHT::readSensor)
//     .property("humidity", &DHT::humidity)
//     .property("temperature", &DHT::temperature)
// }

void check_interrupt_fn(void *dummy) {
    R_CheckUserInterrupt(); 
}

int pending_interrupt() {
    return !(R_ToplevelExec(check_interrupt_fn, NULL));
}



extern "C" {
int myDHT(double* tempOut, double* humidOut, double* samplingTime, double* rD) {
    DHT dht;
    int chk;
    unsigned int readDelay = *rD;
    // clock_t start,end;
    printf("readDelay value: %u\n",readDelay);
    // start = clock();
    chk = dht.readDHT11(DHT11_Pin,readDelay);	//read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
    // end = clock();
    // samplingTime[0] = ((double) (end-start))/CLOCKS_PER_SEC;
    // std::cout <<"Time taken by function:" << samplingTime[0] << " microseconds"<< std::endl;
    if(chk == DHTLIB_OK) printf("DHT11,OK! \n");
    tempOut[0] = dht.temperature;
    humidOut[0] = dht.humidity;

    return 1;
}
}
// int main(double* out) {
//     DHT dht;
//     int chk;
//     int counts = 0;
//
//     printf("Program is starting ...\n");
//
//     while (!pending_interrupt()) {
//         counts++;
//         printf("Measurement counts : %d \n", counts);
//         for (int i = 0; i < 15; i++){
//             chk = dht.readDHT11(DHT11_Pin);	//read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
//             if(chk == DHTLIB_OK){
//                 printf("DHT11,OK! \n");
//                 break;
//             }
//             delay(100);
//         }
//         out[0] = dht.humidity;
//         printf("Humidity is %.2f %%, \t Temperature is %.2f *C\n\n",dht.humidity, dht.temperature);
//         delay(DELAY);
//     }
//
//     return 1;
// }


