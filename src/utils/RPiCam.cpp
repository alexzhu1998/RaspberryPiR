#include "RPiCam.h"

RPiCam::RPiCam() {
    timeBetweenAcquisition = 1000;
}

int RPiCam_Operator::initiate_camera(raspicam::RaspiCam &Camera) {
    
    Camera.setFormat(fmt);
	Camera.setCaptureSize(w,h);
    //Open camera 
	std::cout<<"Opening Camera..."<<std::endl;
    if ( !Camera.open()) {std::cerr<<"Error opening camera"<<std::endl;return FAILURE;}


    len = Camera.getImageBufferSize();
    
    //wait a while until camera stabilizes
    std::cout<<"Sleeping for 3 secs"<<std::endl;
	sleep(3);

    
    return SUCCESS;
}

int RPiCam_Operator::capture(raspicam::RaspiCam &Camera) {
    data = new unsigned char[len];
    //capture
	Camera.grab();
	
	//extract the image in predefined format
	Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_IGNORE );//get camera image
    
    v = std::vector<int>(len);
    for (int i = 0; i < len;i++) {
        v[i] = static_cast<int>(data[i]);
    }
    

    return SUCCESS;
}

void RPiCam_Operator::saveImage (raspicam::RaspiCam &Camera) {
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

