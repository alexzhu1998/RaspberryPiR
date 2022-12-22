#include "RPiCam.h"



int RPiCam_Operator::initiate_camera(raspicam::RaspiCam &Camera) {
    
    Camera.setFormat(fmt);
	Camera.setCaptureSize(w,h);
    //Open camera 
	Rcpp::Rcout<<"Opening Camera..."<<std::endl;
    if ( !Camera.open()) {Rcpp::Rcerr<<"Error opening camera"<<std::endl;return FAILURE;}


    len = Camera.getImageBufferSize();
    
    //wait a while until camera stabilizes
    Rcpp::Rcout<<"Sleeping for 3 secs"<<std::endl;
	sleep(3);

    
    return SUCCESS;
}

int RPiCam_Operator::capture(raspicam::RaspiCam &Camera) {
    data = new unsigned char[len]; // bug probably have something to do with this data type
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

int RPiCam_Operator::capture_string(raspicam::RaspiCam &Camera) {
    data = new unsigned char[len]; // bug probably have something to do with this data type
    //capture
	Camera.grab();
	
	//extract the image in predefined format
	Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_IGNORE );//get camera image
    
    s = std::string(reinterpret_cast<char const*>(data), len);
    delete data;
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
    Rcpp::Rcout<<"Image saved at " << filepath.c_str()<<std::endl;
}

// void RPiCam::writeMemory(int len) {
//     open_write();
//     map_write();
//     int i = 0; 
//     int chk;
//     TimeVar start;
//     RPiCam_Operator rpicam = RPiCam_Operator(raspicam::RASPICAM_FORMAT_GRAY,1,1,"/home/pi/Pictures/raspicam_image.ppm");
//     raspicam::RaspiCam Camera;
//     rpicam.initiate_camera(Camera);

//     for (;!pending_interrupt();i++) {
//         if (i == CAMBLOCKLENGTH) {
//             i = 0;
//             ptr_obj->complete = true;
//         }
//         Rcpp::Rcout << "Block No: " << i << std::endl;
//         start = timeNow();
//         // assigning values
//         chk = rpicam.capture_string(Camera);
//         data_obj->raw_time[i] = get_raw_time();
//         // data_obj->cam_data[i] = rpicam.s;
        
//         double elapsed = intervalDuration(timeNow()-start);
        
//         Rcpp::Rcout << "Head of image is " << data_obj->cam_data[i].substr(0,50) << std::endl;
//         millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
//         Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
//         ptr_obj->index = i;
//     }
//     Rcpp::Rcout << "End Writing" << std::endl;
//     Rcpp::Rcout << "Freeing Memory" << std::endl;
//     SharedMemory::freeMemory();
// }

// Rcpp::List RPiCam::readMemory(int n) {
//     init_read();
//     return Rcpp::List::create(
//         Rcpp::Named("x") = Rcpp::NumericVector(1)
//     );
// }
