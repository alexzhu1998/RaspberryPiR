#include "shared_memory.h"


DataBlock::DataBlock(int _num_data_points, int _block_length,int type) {
    if (type == REGULAR_SENSOR_TYPE) {
        // block size refers to how many data points will be gathered per iteration
        sensor_data = new double[_num_data_points*_block_length];
    } else if (type == RASPI_CAMERA_TYPE) {
        // block size refers to how many pixels will be gathered per iteration
        cam_data = new uint8_t[_num_data_points*_block_length]; // need changes to add an entire block of cam_data
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
    allocated_memory = sizeof(time_t)*_block_length + sizeof(uint8_t)*_block_length*_w*_h*_channels + sizeof(int) + sizeof(DataBlock);
}

DataPtr::DataPtr(int _num_data_points,int _block_length):block_length(_block_length),num_data_points(_num_data_points) {
    allocated_memory = sizeof(time_t)*_block_length + sizeof(double)*_block_length*_num_data_points + sizeof(int)+ sizeof(DataBlock);
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

void SharedMemory::map_data_ptr(DataPtr* source) {
    dp = static_cast<DataPtr*>(mmap(NULL,sizeof(DataPtr),PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    if (dp == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd_ptr, sizeof(DataPtr)) == -1)           /* To obtain file size */
        handle_error("open_write fd_ptr ftruncate");
    memcpy(dp,source,sizeof(DataPtr));
    if (dp->allocated_memory != -1) {
        Rcpp::Rcout << "Pointer Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_ptr memcpy error");
    }
}

void SharedMemory::map_data_obj(size_t mmap_size, DataBlock* source) {
    db = static_cast<DataBlock*>(mmap(NULL,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (db == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd, mmap_size) == -1)           /* To obtain file size */
        handle_error("open_write fd ftruncate");

    memcpy(db,source,mmap_size);
    
    if (db->success == 1) {
        Rcpp::Rcout << "Data Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_obj memcpy error");
    }

}

void SharedMemory::retrieve_data_ptr(size_t mmap_size) {
    dp = static_cast<DataPtr*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd_ptr,0));
    if (dp == MAP_FAILED)
        handle_error("mmap ptr_obj");
    
}

void SharedMemory::retrieve_data_obj(size_t mmap_size) {
    db = static_cast<DataBlock*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd,0));
    if (db == MAP_FAILED)
        handle_error("mmap db");
    
    if (db->success == 1) {
        Rcpp::Rcout << "Data Object successfully captured" << std::endl;
    } else {
        handle_error("map_data_obj memcpy error");
    }
    // ptr_obj = static_cast<DataPtr*>(mmap(NULL,sizeof(*ptr_obj), PROT_READ,MAP_SHARED,fd_ptr,0));
    // Rcpp::Rcout <<"init_read ptr_obj->index: " << ptr_obj->index << std::endl;

    // Rcpp::Rcout <<"Temperature: " << db->data1[0]  << "Humidity: " << db->data2[0]<< std::endl;
    // if (ptr_obj == MAP_FAILED)
    //     handle_error("mmap ptr_obj");
    
}


void SharedMemory::freeMemory() {
    shm_unlink(shmpath);
    shm_unlink(shmpath_ptr);
}





