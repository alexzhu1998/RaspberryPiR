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

#define RPICAM_SHM_PATH          "/shmpath"
#define RPICAM_SHM_PTR_PATH      "/shmpath_ptr"

#define CAMBLOCKLENGTH 2


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
        std::string s; //avoid using std::string, use C-style arrays if you already know the size
        int initiate_camera(raspicam::RaspiCam &Camera);
        int capture(raspicam::RaspiCam &Camera);
        int capture_string(raspicam::RaspiCam &Camera);
        void saveImage (raspicam::RaspiCam &Camera);
};


class RPiCam: public Sensor, public SharedMemory {
    public:
        RPiCam(const char* rpicam_shmpath, const char* rpicam_shmpath_ptr, int writeFlag): SharedMemory(rpicam_shmpath,rpicam_shmpath_ptr,writeFlag) {
            timeBetweenAcquisition = 1000;
            
        }
        
        void info();
        void writeMemory();
        Rcpp::List readMemory(int n);
        void killProcess();
};

#endif //RPI_CAM_H