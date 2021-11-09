#ifndef RPI_CAM_H
#define RPI_CAM_H

#include "sensors_control.h"
#include "shared_memory.h"

#include <ctime>
#include <fstream>
#include <raspicam/raspicam.h>




#define SUCCESS 1
#define FAILURE 0

#define RASPICAM_SHM_PATH          "/raspicam"
#define RASPICAM_SHM_PTR_PATH      "/raspicam_ptr"



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
        size_t image_len;
        std::vector<int> v;
        std::string s; //avoid using std::string, use C-style arrays if you already know the size
        int initiate_camera(raspicam::RaspiCam &Camera);
        int capture(raspicam::RaspiCam &Camera);
        int capture_string(raspicam::RaspiCam &Camera);
        int capture_array(raspicam::RaspiCam &Camera, CameraBlock* cb, int offset);
        void saveImage (raspicam::RaspiCam &Camera);
};


class RPiCam: public Sensor{
    public:
        RPiCam (unsigned int width,unsigned int height,unsigned int channels) {
            timeBetweenAcquisition = 1000;
            image_len = (size_t)width*height*channels;
        }
        size_t image_len;
        void info();
        void writeMemory();
        Rcpp::List readMemory(int n);
        void killProcess();
};

#endif //RPI_CAM_H