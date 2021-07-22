#include <ctime>
#include <fstream>
#include <iostream>
#include <Rcpp.h>
#include <unistd.h>
#include <raspicam/raspicam.h>
#include "shared_memory.h"
#include "imageLibRaspicam/imageLib.h"



#define KEY_2 3456

// [[Rcpp::export]]
void simpleCapture(Rcpp::CharacterVector path = "/home/pi/Pictures/raspicam_image.ppm") {
	raspicam::RaspiCam Camera;
    std::string s = Rcpp::as<std::string>(path[0]);
    initiate_camera(Camera, raspicam::RASPICAM_FORMAT_GRAY,WIDTH,HEIGHT);
    std::cout << "Camera Information:" << std::endl;
    std::cout << "Width: "<< Camera.getWidth() << " Height: " << Camera.getHeight() << std::endl;
	std::cout << "ImageBufferSize: " << Camera.getImageBufferSize() << std::endl;
    unsigned char *data=new unsigned char[Camera.getImageBufferSize()];

    capture(data, Camera);

    saveImage(s,data,Camera);
    delete data;
}

// // [[Rcpp::export]]
// void writeCameraMemory() {
//     raspicam::RaspiCam Camera;
// }