#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "sensors_control.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> //O_CREAT, O_RDWR
#include <vector>


#define BLOCK_LENGTH 200

typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define intervalDuration(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
#define millisleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

class DataBlock {
public:
    std::vector<time_t> raw_time;
    std::vector<double> data1;
    std::vector<double> data2;
    std::vector<std::string> cam_data;
    DataBlock(int _n1,int _n2,int _n3, int _n4, int _image_len) {    
        raw_time = std::vector<time_t>(_n1,0);
        data1 = std::vector<double>(_n2,0);
        data2 = std::vector<double>(_n3,0);
        cam_data = std::vector<std::string>(_n4,std::string(_image_len,42));
    }
};


class DataPtr {
public:
    DataPtr(int _block_length) {
        block_length = _block_length;
    }
    int index = 0;
    bool complete = 0;
    int block_length;
};

class SharedMemory {
public:
    const char* shmpath;
    const char* shmpath_ptr;
    SharedMemory(const char* _shmpath, const char* _shmpath_ptr) {
        shmpath = _shmpath;
        shmpath_ptr = _shmpath_ptr;
        // data_obj = _data_obj;
        // ptr_obj = _ptr_obj;
    }
    int fd;
    int fd_ptr;
    // DataBlock* data_obj;
    // DataPtr* ptr_obj;
    
    void open_write();
    void map_write(DataBlock* data_obj, DataPtr* ptr_obj);
    void init_read(DataBlock* data_obj, DataPtr* ptr_obj);
    void freeMemory();
};

#endif