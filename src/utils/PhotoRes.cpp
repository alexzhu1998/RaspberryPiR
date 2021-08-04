#include "PhotoRes.h"



PhotoRes_Operator::PhotoRes_Operator() {
    wiringPiSetup();
}

int PhotoRes_Operator::readPhotoRes(int pin) {
    pinMode(pin,OUTPUT);
    digitalWrite(pin,LOW);
    delay(100);

    pinMode(pin,INPUT);
    clock_t t= clock(), diff=0;
    
    while (digitalRead(pin) == LOW) {
        diff = clock() - t;
    }
    PhoR_time_to_charge = (double) diff/ CLOCKS_PER_SEC;
    return SENSOR_OK;
}