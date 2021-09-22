#include "shared_memory.h"


DataPtr::DataPtr(int _w,int _h, int _channels, int _block_length): w(_w),h(_h),channels(_channels),block_length(_block_length) {
    image_size = _w*_h*_channels;
    allocated_memory = sizeof(CameraBlock);
    // allocated_memory = sizeof(time_t)*_block_length + sizeof(uint8_t)*_block_length*_w*_h*_channels + sizeof(DataBlock1);
}

DataPtr::DataPtr(int _num_data_points, int _block_length,int dataFlag): num_data_points(_num_data_points),block_length(_block_length) {
    if (dataFlag == 1) {
        allocated_memory = sizeof(DataBlock1);
    } else if (dataFlag == 2) {
        allocated_memory = sizeof(DataBlock2);
    }
    // allocated_memory = sizeof(time_t)*_block_length + sizeof(double)*_block_length*_num_data_points + sizeof(DataBlock1);
}

SharedMemory::SharedMemory(const char* _shmpath, const char* _shmpath_ptr, int writeFlag, int dataFlag):shmpath(_shmpath),shmpath_ptr(_shmpath_ptr),wFlag(writeFlag),dFlag(dataFlag) {
    if (writeFlag==SHM_WRITE) {
        open_write();

        if (dataFlag == DATA_RASPICAM) {
            DataPtr source_dp = DataPtr(WIDTH,HEIGHT,1,CAM_BLOCK_LENGTH);
            map_DataPtr(&source_dp,sizeof(DataPtr));
            CameraBlock source_cb;
            source_cb.success = 1;
            map_CameraBlock(&source_cb,sizeof(CameraBlock));
        } else if (dataFlag == 1) {
            DataPtr source_dp = DataPtr(1,BLOCK_LENGTH,DATA_PHOTORES);
            map_DataPtr(&source_dp,sizeof(DataPtr));
            DataBlock1 source_db1;
            source_db1.success = 1;
            map_DataBlock1(&source_db1,sizeof(DataBlock1));
        } else if (dataFlag == 2) {
            DataPtr source_dp = DataPtr(2,BLOCK_LENGTH,DATA_DHT11);
            map_DataPtr(&source_dp,sizeof(DataPtr));
            DataBlock2 source_db2;
            source_db2.success = 1;
            map_DataBlock2(&source_db2,sizeof(DataBlock2));
        }
    } else if (writeFlag == SHM_READ){
        open_read();
        retrieve_DataPtr(sizeof(DataPtr));
        if (dataFlag == DATA_RASPICAM) {
            retrieve_CameraBlock(sizeof(CameraBlock));
        } else if (dataFlag == 1) {
            retrieve_DataBlock1(sizeof(DataBlock1));
        } else if (dataFlag == 2) {
            retrieve_DataBlock2(sizeof(DataBlock2));
        }
    } else if (writeFlag == SHM_SCAN) {
        open_read();
        retrieve_DataPtr(sizeof(DataPtr));
    }

    
}

SharedMemory::~SharedMemory() {
    if (wFlag == SHM_SCAN) {
        munmap(dp,sizeof(DataPtr));
        close(fd);
        close(fd_ptr);
    } else if (wFlag == SHM_READ) {
        munmap(dp,sizeof(DataPtr));
        if (dFlag == 1) munmap(db1,sizeof(DataBlock1));
        if (dFlag == 2) munmap(db2,sizeof(DataBlock2));
        if (dFlag == DATA_RASPICAM) munmap(cb,sizeof(CameraBlock));
        close(fd);
        close(fd_ptr);
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

void SharedMemory::map_DataPtr(DataPtr* source,size_t mmap_size) {
    dp = static_cast<DataPtr*>(mmap(NULL,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    if (dp == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd_ptr, mmap_size) == -1)           /* To obtain file size */
        handle_error("open_write fd_ptr ftruncate");
    memcpy(dp,source,sizeof(DataPtr));
    if (dp->allocated_memory != 0) {
        Rcpp::Rcout << "Pointer Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_ptr memcpy error");
    }
}

void SharedMemory::map_DataBlock1(DataBlock1* source,size_t mmap_size) {
    db1 = static_cast<DataBlock1*>(mmap(NULL,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (db1 == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd, mmap_size) == -1)           /* To obtain file size */
        handle_error("open_write fd ftruncate");
    memcpy(db1,source,mmap_size);
    
    if (db1->success == 1) {
        Rcpp::Rcout << "Data Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_obj memcpy error");
    }
}

void SharedMemory::map_DataBlock2(DataBlock2* source,size_t mmap_size) {
    db2 = static_cast<DataBlock2*>(mmap(NULL,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (db2 == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd, mmap_size) == -1)           /* To obtain file size */
        handle_error("open_write fd ftruncate");
    memcpy(db2,source,mmap_size);
    
    if (db2->success == 1) {
        Rcpp::Rcout << "Data Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_obj memcpy error");
    }
}

void SharedMemory::map_CameraBlock(CameraBlock* source,size_t mmap_size) {
    cb = static_cast<CameraBlock*>(mmap(NULL,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (cb == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd, mmap_size) == -1)           /* To obtain file size */
        handle_error("open_write fd ftruncate");
    memcpy(cb,source,mmap_size);
    
    if (cb->success == 1) {
        Rcpp::Rcout << "Data Object successfully shared" << std::endl;
    } else {
        handle_error("map_data_obj memcpy error");
    }
}

void SharedMemory::unmap_DataPtr(DataPtr* source) {
    if ( munmap(source,sizeof(DataPtr)) == -1) {
        handle_error("munmap failed with error:");
    }
}

void SharedMemory::unmap_DataBlock1(DataBlock1* source) {
    if ( munmap(source,sizeof(DataBlock1)) == -1) {
        handle_error("munmap failed with error:");
    }
}

void SharedMemory::unmap_DataBlock2(DataBlock2* source) {
    if ( munmap(source,sizeof(DataBlock2)) == -1) {
        handle_error("munmap failed with error:");
    }
}

void SharedMemory::unmap_CameraBlock(CameraBlock* source) {
    if ( munmap(source,sizeof(CameraBlock)) == -1) {
        handle_error("munmap failed with error:");
    }
}

void SharedMemory::retrieve_DataPtr(size_t mmap_size) {
    dp = static_cast<DataPtr*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd_ptr,0));
    if (dp == MAP_FAILED)
        handle_error("mmap ptr_obj");
}

void SharedMemory::retrieve_DataBlock1(size_t mmap_size) {
    db1 = static_cast<DataBlock1*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd,0));
    if (db1 == MAP_FAILED)
        handle_error("mmap data_obj");
    
    if (db1->success != 1) {
        handle_error("map_data_obj memcpy error");
    }
    
}

void SharedMemory::retrieve_DataBlock2(size_t mmap_size) {
    db2 = static_cast<DataBlock2*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd,0));
    if (db2 == MAP_FAILED)
        handle_error("mmap data_obj");
    
    if (db2->success != 1) {
        handle_error("map_data_obj memcpy error");
    }
    
}

void SharedMemory::retrieve_CameraBlock(size_t mmap_size) {
    cb = static_cast<CameraBlock*>(mmap(NULL,mmap_size, PROT_READ,MAP_SHARED,fd,0));
    if (cb == MAP_FAILED)
        handle_error("mmap data_obj");
    
    if (cb->success != 1) {
        handle_error("map_data_obj memcpy error");
    }
    
}

int SharedMemory::retrieve_DataPtrIndex() {
    return dp->cur_index;
}

void SharedMemory::freeMemory() {
    Rcpp::Rcout << "Freed Memory" <<std::endl;
    shm_unlink(shmpath);
    shm_unlink(shmpath_ptr);
}





