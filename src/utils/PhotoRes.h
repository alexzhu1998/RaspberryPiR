#ifndef PHOTO_RES_H
#define PHOTO_RES_H

#include "sensors_control.h"
#include "shared_memory.h"



#define SENSOR_OK                   0

#define PHOTORES_SHM_PATH           "/PHOTORES"
#define PHOTORES_SHM_PTR_PATH       "/PHOTORES_ptr"


class PhotoRes_Operator : public Sensor {
    public:
        PhotoRes_Operator();
        double PhoR_time_to_charge;
        int readPhotoRes(int pin);
};

class PhotoRes: public Sensor {
    public:
        PhotoRes(){
            timeBetweenAcquisition = 1000;
        }
        void info();
        void writeMemory(int pin);
        Rcpp::List readMemory(int n);
        void killProcess();
};


#endif // PHOTO_RES_H