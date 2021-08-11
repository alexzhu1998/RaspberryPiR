#ifndef PHOTO_RES_H
#define PHOTO_RES_H

#include "sensors_control.h"
#include "shared_memory.h"



#define SENSOR_OK               0

class PhotoRes: public Sensor {
    public:
        
        void info();
        void writeMemory(int pin);
        Rcpp::List readMemory(int n);
        void killProcess();
};


class PhotoRes_Operator : public Sensor {
    public:
        PhotoRes_Operator();
        double PhoR_time_to_charge;
        int readPhotoRes(int pin);
};




#endif // PHOTO_RES_H