#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <raspicam/raspicam.h>

#include "imageLib.h"



int capture(unsigned char *data,raspicam::RaspiCam &Camera) {
    
    
    //capture
	Camera.grab();
	
	//extract the image in predefined format
	Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_IGNORE );//get camera image

    return SUCCESS;
}

int initiate_camera(raspicam::RaspiCam &Camera,raspicam::RASPICAM_FORMAT fmt, unsigned int w, unsigned int h) {
    
    Camera.setFormat(fmt);
	Camera.setCaptureSize(w,h);
    //Open camera 
	std::cout<<"Opening Camera..."<<std::endl;
    if ( !Camera.open()) {std::cerr<<"Error opening camera"<<std::endl;return FAILURE;}

    //wait a while until camera stabilizes
    std::cout<<"Sleeping for 3 secs"<<std::endl;
	sleep(3);

    return SUCCESS;
}

void saveImage (std::string filepath, unsigned char *data, raspicam::RaspiCam &Camera) {
    std::ofstream outFile( filepath.c_str(), std::ios::binary);
    if ( 
		Camera.getFormat()==raspicam::RASPICAM_FORMAT_BGR ||
		Camera.getFormat()==raspicam::RASPICAM_FORMAT_RGB 
	) {
        outFile<<"P6\n";
    } else if ( Camera.getFormat()==raspicam::RASPICAM_FORMAT_GRAY ) {
        outFile<<"P5\n";
    } else if ( Camera.getFormat()==raspicam::RASPICAM_FORMAT_YUV420 ) { //made up format
        outFile<<"P7\n";
    }

    outFile<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
    outFile.write ( ( char* ) data, Camera.getImageBufferSize() );
    std::cout<<"Image saved at " << filepath.c_str()<<std::endl;
}

