#include "shared_memory.h"


void SharedMemory::open_write() {
    fd = shm_open(shmpath,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1){
        shm_unlink(shmpath);
        handle_error("open_write fd shm_open");
    }


    fd_ptr = shm_open(shmpath_ptr,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if (fd_ptr == -1) {
        shm_unlink(shmpath_ptr);
        handle_error("open_write fd_ptr shm_open");
    }

    
}

void SharedMemory::map_write(DataBlock* data_obj, DataPtr* ptr_obj) {
    data_obj = static_cast<DataBlock*>(mmap(NULL,sizeof(*data_obj), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (data_obj == MAP_FAILED)
        handle_error("mmap data_obj");
    if (ftruncate(fd, sizeof(*data_obj)) == -1)           /* To obtain file size */
        handle_error("open_write fd ftruncate");
    ptr_obj = static_cast<DataPtr*>(mmap(NULL,sizeof(*ptr_obj), PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));

    if (ptr_obj == MAP_FAILED)
        handle_error("mmap");
    if (ftruncate(fd_ptr, sizeof(*ptr_obj)) == -1)           /* To obtain file size */
        handle_error("open_write fd_ptr ftruncate");
}

void SharedMemory::init_read(DataBlock* data_obj, DataPtr* ptr_obj) {
    // file descriptor
    fd = shm_open(shmpath, O_RDONLY, S_IRUSR|S_IWUSR);
    // Rcpp::Rcout << shmpath << std::endl;
    fd_ptr = shm_open(shmpath_ptr, O_RDONLY, S_IRUSR|S_IWUSR);
    // Rcpp::Rcout << shmpath_ptr << std::endl;
    if (fd == -1){
        handle_error("open_read fd shm_open");
    }

    if (fd_ptr == -1){
        handle_error("open_read fd_ptr shm_open");
    }

    data_obj = static_cast<DataBlock*>(mmap(NULL,sizeof(*data_obj), PROT_READ,MAP_SHARED,fd,0));
    if (data_obj == MAP_FAILED)
        handle_error("mmap data_obj");
    
    ptr_obj = static_cast<DataPtr*>(mmap(NULL,sizeof(*ptr_obj), PROT_READ,MAP_SHARED,fd_ptr,0));
    Rcpp::Rcout <<"init_read ptr_obj->index: " << ptr_obj->index << std::endl;

    // Rcpp::Rcout <<"Temperature: " << data_obj->data1[0]  << "Humidity: " << data_obj->data2[0]<< std::endl;
    if (ptr_obj == MAP_FAILED)
        handle_error("mmap ptr_obj");
    
}


void SharedMemory::freeMemory() {
    shm_unlink(shmpath);
    shm_unlink(shmpath_ptr);
}





