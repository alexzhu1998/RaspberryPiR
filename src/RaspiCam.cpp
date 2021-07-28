#include <ctime>
#include <fstream>
#include <iostream>
#include <Rcpp.h>
#include <unistd.h>
#include <raspicam/raspicam.h>
#include "shared_memory.h"
#include "imageLibRaspicam/imageLib.h"
#include <time.h> //for time
#include <chrono> // for measuring real execution time
#include <thread> // for sleep


template <typename T, std::size_t N> struct camera_obj {
    T data[N];
    time_t raw_time[N];
    bool complete;
};


// [[Rcpp::export]]
void simpleCapture(Rcpp::CharacterVector path = "/home/pi/Pictures/raspicam_image.ppm") {
	raspicam::RaspiCam Camera;
    std::string s = Rcpp::as<std::string>(path[0]);
    initiate_camera(Camera, raspicam::RASPICAM_FORMAT_GRAY,WIDTH,HEIGHT);
    Rcpp::Rcout << "Camera Information:" << std::endl;
    Rcpp::Rcout << "Width: "<< Camera.getWidth() << " Height: " << Camera.getHeight() << std::endl;
	Rcpp::Rcout << "ImageBufferSize: " << Camera.getImageBufferSize() << std::endl;
    unsigned char *data=new unsigned char[Camera.getImageBufferSize()];

    capture(data, Camera);

    // Experimentation
    double avg_pixel= (double) data[0];
    for (int i = 1; i < Camera.getImageBufferSize(); i++) {
        double frac = (double) (i-1)/(double) i;
        avg_pixel =  (double) data[i]/(double) i + frac*avg_pixel;
        // Rcpp:: Rcout << (int) data[i] << " ";
    }
    Rcpp::Rcout<< std::endl;
    Rcpp::Rcout<< avg_pixel << std::endl;
    
    // saveImage(s,data,Camera);
    delete data;
}


// [[Rcpp::export]]
void freeMemory_RaspiCam() {
    const char* shmpath = "/raspicam";
    const char* shmpath_pointer = "/raspicam_ptr";
    Rcpp::Rcout <<"Free block" << std::endl;
    shm_unlink(shmpath);
    Rcpp::Rcout <<"Free block: pointer"  << std::endl;
    shm_unlink(shmpath_pointer);
}

// [[Rcpp::export]]
void writeMemory_RaspiCam(Rcpp::NumericVector w = 1000) {
    double interval = w[0];
    const char* shmpath = "/raspicam";
    const char* shmpath_pointer = "/raspicam_ptr";
    int fd = shm_open(shmpath,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    int fd_ptr = shm_open(shmpath_pointer,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1 || fd_ptr == -1){
        shm_unlink(shmpath);
        shm_unlink(shmpath_pointer);
        handle_error("open");
    }

    if (ftruncate(fd, sizeof(data_obj<double,BLOCK_LENGTH>)) == -1)           /* To obtain file size */
        handle_error("fstat");
    if (ftruncate(fd_ptr, sizeof(data_ptr)) == -1)           /* To obtain file size */
        handle_error("fstat");

    data_obj<double,BLOCK_LENGTH>* data_block = static_cast<data_obj<double,BLOCK_LENGTH>*>(mmap(NULL,sizeof(*data_block), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (data_block == MAP_FAILED) 
        handle_error("mmap");

    raspicam::RaspiCam Camera;

    initiate_camera(Camera, raspicam::RASPICAM_FORMAT_GRAY,WIDTH,HEIGHT);
    Rcpp::Rcout << "Camera Information:" << std::endl;
    Rcpp::Rcout << "Width: "<< Camera.getWidth() << " Height: " << Camera.getHeight() << std::endl;
	Rcpp::Rcout << "ImageBufferSize: " << Camera.getImageBufferSize() << std::endl;
    
    
    unsigned char *data=new unsigned char[Camera.getImageBufferSize()];
    TimeVar start;
    data_ptr* status = static_cast<data_ptr*>(mmap(NULL,sizeof(*status), PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    int i = 0;
    for (;!pending_interrupt();++i) {
        if (i == BLOCK_LENGTH) {
            i = 0;
            status->complete = true;
        }
        
        start = timeNow();
        
        capture(data, Camera);
        double avg_pixel= (double) data[0];
        for (int j = 1; j < Camera.getImageBufferSize(); j++) {
            double frac = (double) (j-1)/(double) j;
            avg_pixel =  (double) data[j]/(double) j + frac*avg_pixel;
            // Rcpp:: Rcout << (int) data[j] << " ";
        }
        data_block->data1[i] = avg_pixel;
        data_block->raw_time[i] = get_raw_time();
        Rcpp::Rcout << "Average Pixel Value: " << avg_pixel  << std::endl;
        double elapsed = intervalDuration(timeNow()-start);
        
        millisleep((unsigned int)std::max((interval-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        status->index = i;
    }

    Rcpp::Rcout << "End Writing" << std::endl;


    freeMemory_RaspiCam();
    
}

// [[Rcpp::export]]
Rcpp::List readMemory_RaspiCam(Rcpp::IntegerVector read_block = 1) {
    int n = read_block[0];
    if(n > BLOCK_LENGTH) {
        Rcpp::stop("size of reading blocks is greater than memory capacity.");
    }
    const char* shmpath = "/raspicam";
    const char* shmpath_pointer = "/raspicam_ptr";
    int fd = shm_open(shmpath, O_RDWR, S_IRUSR|S_IWUSR);
    int fd_ptr = shm_open(shmpath_pointer, O_RDWR, S_IRUSR|S_IWUSR);

    data_ptr* status = static_cast<data_ptr*>(mmap(NULL,sizeof(*status), PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    if (status == MAP_FAILED) 
        handle_error("mmap");
    
    int cur = status->index;
    
    struct data_obj<double,BLOCK_LENGTH>* data_block = static_cast<data_obj<double,BLOCK_LENGTH>*>(mmap(NULL,sizeof(*data_block), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));

    char time_string[DATE_STRING_SIZE];
    Rcpp::StringVector datetime;
    Rcpp::NumericVector avg_pixel;

    for (int i =0 ; i < n;++i) {
        if (cur-i < 0) {
            if (status->complete) {
                cur = BLOCK_LENGTH+i-1;
            } else {
                Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                break;
            }
        }
        convert_to_string(time_string,data_block->raw_time[cur-i]);
        datetime.push_back(time_string);
        avg_pixel.push_back(data_block->data1[cur-i]);
    }

    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("avg_pixel") = avg_pixel
    );
}