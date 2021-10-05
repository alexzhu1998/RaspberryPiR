#ifndef PHOTO_RES_H
#define PHOTO_RES_H

#include "sensors_control.h"
#include "shared_memory.h"



#define PHOR_SENSOR_OK                   0

#define PHOTORES_SHM_PATH           "/PHOTORES"
#define PHOTORES_SHM_PTR_PATH       "/PHOTORES_ptr"


class PhotoRes_Operator : public Sensor {
    private:
        int pin;
    public:
        PhotoRes_Operator(int _pin);
        double PhoR_time_to_charge;
        int readPhotoRes();
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