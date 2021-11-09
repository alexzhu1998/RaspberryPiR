#include "RPiCam.h"



int RPiCam_Operator::initiate_camera(raspicam::RaspiCam &Camera) {
    
    Camera.setFormat(fmt);
	Camera.setCaptureSize(w,h);
    //Open camera 
	std::cout<<"Opening Camera..."<<std::endl;
    if ( !Camera.open()) {std::cerr<<"Error opening camera"<<std::endl;return FAILURE;}


    image_len = Camera.getImageBufferSize();
    
    //wait a while until camera stabilizes
    std::cout<<"Sleeping for 3 secs"<<std::endl;
	sleep(3);

    
    return SUCCESS;
}

int RPiCam_Operator::capture(raspicam::RaspiCam &Camera) {
    data = new unsigned char[image_len]; // bug probably have something to do with this data type
    //capture
	Camera.grab();
	
	//extract the image in predefined format
	Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_IGNORE );//get camera image
    
    v = std::vector<int>(image_len);
    for (int i = 0; i < image_len;i++) {
        v[i] = static_cast<int>(data[i]);
    }
    

    return SUCCESS;
}

int RPiCam_Operator::capture_string(raspicam::RaspiCam &Camera) {
    data = new unsigned char[image_len]; // bug probably have something to do with this data type
    //capture
	Camera.grab();
	
	//extract the image in predefined format
	Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_IGNORE );//get camera image
    
    s = std::string(reinterpret_cast<char const*>(data), image_len);
    delete[] data;
    return SUCCESS;
}

int RPiCam_Operator::capture_array(raspicam::RaspiCam &Camera, CameraBlock* cb, int offset) {
    data = new unsigned char[image_len]; // bug probably have something to do with this data type
    //capture
	Camera.grab();
	
	//extract the image in predefined format
	Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_IGNORE );//get camera image
    memcpy(cb->data+offset,data,image_len);
    
    delete[] data;
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

void RPiCam::writeMemory() {
    SharedMemory sharedmem(RASPICAM_SHM_PATH,RASPICAM_SHM_PTR_PATH,SHM_WRITE,DATA_RASPICAM);
    DataPtr* data_ptr = sharedmem.dp;
    CameraBlock* data_obj = sharedmem.cb;
    int i = 0; 
    int chk;
    TimeVar start;
    RPiCam_Operator rpicam = RPiCam_Operator(raspicam::RASPICAM_FORMAT_GRAY,1,1,"/home/pi/Pictures/raspicam_image.ppm");
    raspicam::RaspiCam Camera;
    rpicam.initiate_camera(Camera);

    for (;!pending_interrupt();i++) {
        if (i == CAM_BLOCK_LENGTH) {
            i = 0;
            data_ptr->complete = true;
        }
        Rcpp::Rcout << "Block No: " << i << std::endl;
        start = timeNow();
        // assigning values
        
        data_obj->raw_time[i] = get_raw_time();
        
        rpicam.capture_array(Camera,data_obj,i*image_len);
        
        double elapsed = intervalDuration(timeNow()-start);
        
        Rcpp::Rcout << "Head of image is ";
        for (int j =0 ; j < 50; j++) {
            Rcpp::Rcout << data_obj->data[i*image_len+j];
        }
        Rcpp::Rcout << std::endl;
        
        millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        data_ptr->cur_index = i;
    }
    Rcpp::Rcout << "End Writing" << std::endl;
    Rcpp::Rcout << "Freeing Memory" << std::endl;
    sharedmem.freeMemory();
}

Rcpp::List RPiCam::readMemory(int n) {
    SharedMemory sharedmem(RASPICAM_SHM_PATH,RASPICAM_SHM_PTR_PATH,SHM_READ,DATA_RASPICAM);
    DataPtr* data_ptr = sharedmem.dp;
    CameraBlock* data_obj = sharedmem.cb;

    Rcpp::CharacterVector datetime;
    Rcpp::IntegerMatrix image_list(image_len,n);
    int cur = data_ptr->cur_index;
    int block_length = data_ptr->block_length;

    for (int i =0; i < n; ++i) {
        // Index is cur-i, current column index is i
        if (cur-i < 0) {
            if (data_ptr->complete) {
                cur = block_length+i-1;
            } else {
                Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                break;
            }
        }

        std::string t = to_time_string(data_obj->raw_time[cur-i]);
        // initiate unsigned char pointer to copy from mmap file
        unsigned char* temp_data = new unsigned char[image_len];
        // copy from mmap file
        memcpy(temp_data,data_obj->data+ (cur-i)*image_len,image_len);
        // convert to unsigned int
        int* numeric_data = new int[image_len];
        
        for (int j = 0; j < image_len;j++) {
            numeric_data[j] = static_cast<int>(temp_data[j]);
        }
        
        // copy into Rcpp::NumericMatrix 
        image_list(Rcpp::_ ,i) = Rcpp::clone(Rcpp::IntegerVector(numeric_data, numeric_data + image_len));
        delete[] temp_data;
        delete[] numeric_data;

        datetime.push_back(t);
        Rcpp::Rcout << "Index: " << cur - i << " Time: "<< t  << std::endl;
        Rcpp::Rcout << "Head of image: ";
        for (int j = 0; j < 50; j++) {
            Rcpp::Rcout << image_list(j,i) << " ";
        }
        Rcpp::Rcout << std::endl;
    }
    // sharedmem.unmap_DataPtr(data_ptr);
    // sharedmem.unmap_CameraBlock(data_obj);
    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("image") = image_list
    );
}

// Backup output format is unsigned char
// Rcpp::List RPiCam::readMemory(int n) {
//     SharedMemory sharedmem(RASPICAM_SHM_PATH,RASPICAM_SHM_PTR_PATH,SHM_READ,DATA_RASPICAM);
//     DataPtr* data_ptr = sharedmem.dp;
//     CameraBlock* data_obj = sharedmem.cb;

//     Rcpp::CharacterVector datetime;
//     Rcpp::CharacterVector image;
//     int cur = data_ptr->cur_index;
//     int block_length = data_ptr->block_length;

//     for (int i =0; i < n; ++i) {
//         if (cur-i < 0) {
//             if (data_ptr->complete) {
//                 cur = block_length+i-1;
//             } else {
//                 Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
//                 break;
//             }
//         }

//         std::string t = to_time_string(data_obj->raw_time[cur-i]);
//         unsigned char* temp_data = new unsigned char[image_len];
//         memcpy(temp_data,data_obj->data+ (cur-i)*image_len,image_len);
//         std::string s = std::string(reinterpret_cast<char const*>(temp_data), image_len);

//         delete[] temp_data;
//         datetime.push_back(t);
//         image.push_back(s);
//         Rcpp::Rcout << "Index: " << cur - i << " Time: "<< t  << std::endl;
//         Rcpp::Rcout << "Head of image: " <<  s.substr(0,50) << std::endl;
//     }
//     // sharedmem.unmap_DataPtr(data_ptr);
//     // sharedmem.unmap_CameraBlock(data_obj);
//     return Rcpp::List::create(
//         Rcpp::Named("datetime") = datetime,
//         Rcpp::Named("image") = image
//     );
// }