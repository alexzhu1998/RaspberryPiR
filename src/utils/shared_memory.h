#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "sensors_control.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> //O_CREAT, O_RDWR
#include <vector>



#define BLOCK_LENGTH                1000
#define CAM_BLOCK_LENGTH            50

#define WIDTH 	                    320
#define HEIGHT 	                    240
#define CHANNELS                    1

#define REGULAR_SENSOR_TYPE         0
#define RASPI_CAMERA_TYPE           1

// Represents how many data blocks needed except for RaspiCam

#define DATA_RASPICAM               0
#define DATA_PHOTORES               1
#define DATA_DHT11                  2
#define DATA_MQ2                    2
#define DATA_FREE                   -1
           
#define SHM_READ                    0
#define SHM_WRITE                   1
#define SHM_FREE                    2
#define SHM_SCAN                    3


typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define intervalDuration(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
#define millisleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

// class DataBlock {
// public:
//     std::vector<time_t> raw_time;
//     std::vector<double> data1;
//     std::vector<double> data2;
//     std::vector<std::string> cam_data;
//     DataBlock(int _n1,int _n2,int _n3, int _n4, int _image_len) {    
//         raw_time = std::vector<time_t>(_n1,0);
//         data1 = std::vector<double>(_n2,0);
//         data2 = std::vector<double>(_n3,0);
//         cam_data = std::vector<std::string>(_n4,std::string(_image_len,'a'));
//         Rcpp::Rcout << "DataBlock Constructor"  << std::endl;
//         Rcpp::Rcout << sizeof(raw_time) << " " << sizeof(data1) << " " << sizeof(data2)<<" "<< sizeof(cam_data) << std::endl;
//         data1[0] = 15341;
//     }
// };




// class DataPtr {
// public:
//     DataPtr(int _block_length) {
//         block_length = _block_length;
//     }
//     int index = 0;
//     bool complete = 0;
//     int block_length;
// };




struct DataBlock1 {
    time_t raw_time[BLOCK_LENGTH];
    double data[BLOCK_LENGTH];
    bool success;
};

struct DataBlock2 {
    time_t raw_time[BLOCK_LENGTH];
    double data1[BLOCK_LENGTH];
    double data2[BLOCK_LENGTH];
    bool success;
};

struct CameraBlock {
    time_t raw_time[CAM_BLOCK_LENGTH];
    unsigned char data[WIDTH*HEIGHT*CHANNELS*CAM_BLOCK_LENGTH];
    bool success;
};

struct DataPtr {
    int cur_index = 0;
    bool complete = 0;
    size_t allocated_memory = 0;
    int block_length;
    int w;
    int h;
    int channels;
    int num_data_points;
    int image_size;

    DataPtr(int _w, int _h, int _channels,int _block_length);
    DataPtr(int _num_data_points,int _block_length, int dataFlag);
};

class SharedMemory {
private:
    int fd;
    int fd_ptr;
    const char* shmpath;
    const char* shmpath_ptr;
    int dFlag;
    int wFlag;
    void open_write();
    void open_read();
public:
    SharedMemory(const char* _shmpath, const char* _shmpath_ptr,int writeFlag, int dataFlag);
    ~SharedMemory();
    DataPtr* dp;
    DataBlock1* db1;
    DataBlock2* db2;
    CameraBlock* cb;
    
    void map_DataPtr(DataPtr* source,size_t mmap_size);
    void map_DataBlock1(DataBlock1* source,size_t mmap_size);
    void map_DataBlock2(DataBlock2* source,size_t mmap_size);
    void map_CameraBlock(CameraBlock* source,size_t mmap_size);
    
    void unmap_DataPtr(DataPtr* source);
    void unmap_DataBlock1(DataBlock1* source);
    void unmap_DataBlock2(DataBlock2* source);
    void unmap_CameraBlock(CameraBlock* source);

    void retrieve_DataPtr(size_t mmap_size);
    void retrieve_DataBlock1(size_t mmap_size);
    void retrieve_DataBlock2(size_t mmap_size);
    void retrieve_CameraBlock(size_t mmap_size);
    
    int retrieve_DataPtrIndex();
    // void init_read();
    void freeMemory();
    
};

#endif