#include "shared_memory.h"


DataBlock::DataBlock(int _block_size, int _block_length,int type) {
    if (type == REGULAR_SENSOR_TYPE) {
        // block size refers to how many data points will be gathered per iteration
        sensor_data = new double[_block_size*_block_length];
    } else if (type == RASPI_CAMERA_TYPE) {
        // block size refers to how many pixels will be gathered per iteration
        cam_data = new uint8_t[_block_size*_block_length]; // need changes to add an entire block of cam_data
    }
    raw_time = new time_t[_block_length];
    success = 1;
}


DataBlock::~DataBlock() {
    delete[] cam_data;
    delete[] sensor_data;
    delete[] raw_time;
}

DataPtr::DataPtr(int _w, int _h, int _channels,int _block_length):block_length(_block_length),w(_w),h(_h),channels(_channels) {
    image_size = _w*_h*_channels;
    allocated_memory = sizeof(time_t)*_block_length + sizeof(uint8_t)*_block_length*_w*_h*_channels + sizeof(DataBlock);
}

DataPtr::DataPtr(int _num_data_points,int _block_length):block_length(_block_length),num_data_points(_num_data_points) {
    allocated_memory = sizeof(time_t)*_block_length + sizeof(double)*_block_length*_num_data_points + sizeof(DataBlock);
}


SharedMemory::SharedMemory(const char* _shmpath, const char* _shmpath_ptr, int writeFlag):shmpath(_shmpath),shmpath_ptr(_shmpath_ptr) {
    if (writeFlag==SHM_WRITE) {
        open_write();
    } else if (writeFlag == SHM_READ){
        open_read();
    }
    
}

void SharedMemory::open_write() {
    fd = shm_open(shmpath,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    fd_ptr = shm_open(shmpath_ptr,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1 || fd_ptr == -1){
        shm_unlink(shmpath);
        shm_unlink(shmpath_ptr);
        handle_error("open_write fd shm_open");
    }
}

void SharedMemory::open_read() {
    fd = shm_open(shmpath, O_RDONLY, S_IRUSR|S_IWUSR);
    fd_ptr = shm_open(shmpath_ptr, O_RDONLY, S_IRUSR|S_IWUSR);
    if (fd == -1 || fd_ptr == -1){
        handle_error("open_read fd shm_open");
    }
}

void SharedMemory::map_data_ptr(size_t mmap_size, DataPtr* source_data_ptr) {
    data_ptr = static_cast<DataPtr*>(mmap(NULL,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    if (data_ptr == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd_ptr, mmap_size) == -1)           /* To obtain file size */
        handle_error("open_write fd_ptr ftruncate");
    memcpy(data_ptr,source_data_ptr, mmap_size);
    if (data_ptr->allocated_memory != -1) {
        Rcpp::Rcout << "Pointer Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_ptr memcpy error");
    }
}

void SharedMemory::map_data_obj(size_t mmap_size, DataBlock* source_data_obj) {
    data_obj = static_cast<DataBlock*>(mmap(NULL,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (data_obj == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd, mmap_size) == -1)           /* To obtain file size */
        handle_error("open_write fd ftruncate");

    memcpy(data_obj,source_data_obj,mmap_size);
    
    if (data_obj->success == 1) {
        Rcpp::Rcout << "Data Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_obj memcpy error");
    }
    //  *data_obj has size 48
    // Rewrite the data_obj
    
    
    // data_obj = static_cast<DataBlock*>(mmap(NULL,sizeof(*data_obj), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    
    // if (data_obj == MAP_FAILED)
    //     handle_error("mmap data_obj");
    // if (ftruncate(fd, sizeof(*data_obj)) == -1)           /* To obtain file size */
    //     handle_error("open_write fd ftruncate");
    // ptr_obj = static_cast<DataPtr*>(mmap(NULL,sizeof(*ptr_obj), PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    // if (ptr_obj == MAP_FAILED)
    //     handle_error("mmap");
    // if (ftruncate(fd_ptr, sizeof(*ptr_obj)) == -1)           /* To obtain file size */
    //     handle_error("open_write fd_ptr ftruncate");
}

void SharedMemory::retrieve_data_ptr(size_t mmap_size) {
    data_ptr = static_cast<DataPtr*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd_ptr,0));
    if (data_ptr == MAP_FAILED)
        handle_error("mmap ptr_obj");
    
}

void SharedMemory::retrieve_data_obj(size_t mmap_size) {
    data_obj = static_cast<DataBlock*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd,0));
    if (data_obj == MAP_FAILED)
        handle_error("mmap data_obj");
    
    if (data_obj->success == 1) {
        Rcpp::Rcout << "Data Object successfully captured" << std::endl;
    } else {
        handle_error("map_data_obj memcpy error");
    }
    // ptr_obj = static_cast<DataPtr*>(mmap(NULL,sizeof(*ptr_obj), PROT_READ,MAP_SHARED,fd_ptr,0));
    // Rcpp::Rcout <<"init_read ptr_obj->index: " << ptr_obj->index << std::endl;

    // Rcpp::Rcout <<"Temperature: " << data_obj->data1[0]  << "Humidity: " << data_obj->data2[0]<< std::endl;
    // if (ptr_obj == MAP_FAILED)
    //     handle_error("mmap ptr_obj");
    
}


void SharedMemory::freeMemory() {
    shm_unlink(shmpath);
    shm_unlink(shmpath_ptr);
}




