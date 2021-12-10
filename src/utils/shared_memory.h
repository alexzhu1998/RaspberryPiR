#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "sensors_control.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> //O_CREAT, O_RDWR
#include <vector>


#define BLOCK_LENGTH                50

#define REGULAR_SENSOR_TYPE         0
#define RASPI_CAMERA_TYPE           1

#define SHM_READ                    0
#define SHM_WRITE                   1
#define SHM_FREE                    2


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



struct DataBlock {
    time_t* raw_time = NULL;
    double* sensor_data = NULL;
    uint8_t* cam_data = NULL;
    int success = 0;
    // Regular Sensor/Image constructor
    DataBlock(int _num_data_points, int _block_length, int type);
    
    // Destructor
    ~DataBlock();
    
};

struct DataPtr {
    int cur_index;
    bool complete = 0;
    size_t allocated_memory = -1;
    int block_length;
    int w;
    int h;
    int channels;
    int num_data_points;
    int image_size;

    DataPtr(int _w, int _h, int _channels,int _block_length);
    DataPtr(int _num_data_points,int _block_length);
};

class SharedMemory {
private:
    int fd;
    int fd_ptr;
    
public:
    const char* shmpath;
    const char* shmpath_ptr;
    SharedMemory(const char* _shmpath, const char* _shmpath_ptr,int writeFlag);
    
    DataPtr* dp;
    DataBlock* db;
    
    void open_write();
    void open_read();
    void map_data_obj(size_t mmap_size, DataBlock* source);
    void map_data_ptr(DataPtr* source);
    void retrieve_data_obj(size_t mmap_size);
    void retrieve_data_ptr(size_t mmap_size);
    
    void init_read();
    void freeMemory();
};

#endif