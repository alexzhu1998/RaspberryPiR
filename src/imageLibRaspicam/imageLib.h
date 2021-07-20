#include <ctime>
#include <unistd.h>
#include <raspicam/raspicam.h>

#define WIDTH 	320
#define HEIGHT 	240

#define SUCCESS 1
#define FAILURE 0

int capture(unsigned char *data,raspicam::RaspiCam &Camera);
int initiate_camera(raspicam::RaspiCam &Camera,raspicam::RASPICAM_FORMAT fmt, unsigned int w, unsigned int h);
void saveImage (std::string filepath, unsigned char *data, raspicam::RaspiCam &Camera);