#ifndef RPI_CAM_H
#define RPI_CAM_H

#include "sensors_control.h"
#include "shared_memory.h"

#include <ctime>
#include <fstream>
#include <raspicam/raspicam.h>

#define WIDTH 	320
#define HEIGHT 	240


#define SUCCESS 1
#define FAILURE 0

class RPiCam: public Sensor {
    public:
        RPiCam();
        void info();
        void writeMemory(int pin);
        Rcpp::List readMemory(int n);
        void killProcess();
};


class RPiCam_Operator: public Sensor {
    private:
        raspicam::RASPICAM_FORMAT fmt;
        unsigned int w;
        unsigned int h;
        std::string filepath;
        

    public:
        RPiCam_Operator(raspicam::RASPICAM_FORMAT f, unsigned int width,unsigned int height,std::string path) {
            fmt = f;
            w = width*WIDTH;
            h = height*HEIGHT;
            filepath = path;
        };
        unsigned char *data;
        size_t len;
        std::vector<int> v;
        int initiate_camera(raspicam::RaspiCam &Camera);
        int capture(raspicam::RaspiCam &Camera);
        void saveImage (raspicam::RaspiCam &Camera);
};


#endif //RPI_CAM_H