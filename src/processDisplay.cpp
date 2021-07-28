/*
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <vector>
#include "shared_memory.h"
#include "processDisplay.h"

std::vector<std::string> title = {"Process Name","Sensor Id", "Shm Path","GPIO Port", "Status"};


bool check_monitor_created() {
    const char* shmpath = "/MonitorProcess";
    int fd = shm_open(shmpath,O_RDWR, S_IRUSR);
    if (fd == -1){
        return true;
    }
    return false;
}

bool initiate_monitor() {
    const char* shmpath = "/MonitorProcess";
    int n = title.size();
    monitor_obj<MAX_SENSOR,n> monitor;
    int fd = shm_open(shmpath,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1){
        handle_error("initiate_monitor");
        return false;
    }
    if (ftruncate(fd, sizeof(monitor_obj<MAX_SENSOR,n>)) == -1){           // To obtain file size 
        handle_error("fstat");
        return false;
    }

    monitor_obj<MAX_SENSOR,n>* monitor_block = static_cast<monitor_obj<MAX_SENSOR,n>*>(mmap(NULL,sizeof(*data_block), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (monitor_block == MAP_FAILED) {
        handle_error("mmap");
        return false;
    }
    Rcpp::Rcout<< "Success! Monitor initiated!" << std::endl;
    return true;
}

Rcpp::List display_monitor() {
    
}

bool add_process(std::vector<std::string> new_data) {
    if (!check_monitor_created()) {
        Rcpp::stop("Monitor not created!");
    }
    const char* shmpath = "/MonitorProcess";
    int fd = shm_open(shmpath, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1){
        handle_error("add_process");
        return false;
    }
    if (ftruncate(fd, sizeof(monitor_obj<MAX_SENSOR,n>)) == -1){           // To obtain file size 
        handle_error("fstat");
        return false;
    }
    monitor_obj<MAX_SENSOR,n>* monitor_block = static_cast<monitor_obj<MAX_SENSOR,n>*>(mmap(NULL,sizeof(*data_block), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (monitor_block == MAP_FAILED) {
        handle_error("mmap");
        return false;
    }
    int cur_index = monitor_block->num_sensor;
    for (int i = 0; i < title.size();i++) {
        monitor_block->data[cur_index][i] = new_data[i];
    }

    return true;

}

bool remove_process() {

}


int main() {
    // titles
    
    printRow(title);
    Rcpp::Rcout << std::string(nameWidth * title.size() + 3*title.size(),'-') << std::endl;
    
    // display each existing process in the shared memory object
    // ################## TO DO ########################


    // processes
    std::vector<std::string> dht = {"DHT11","0","/DHT11", "GPIO17","Closed"};
    printRow(dht);
    
    // processes
    std::vector<std::string> phor = {"PhotoResistor","1", "/PhoR", "GPIO18","Closed"};
    printRow(phor);

    // processes
    std::vector<std::string> rasp_cam = {"RaspberryPi Cam","2","/raspicam", "NA","Closed"};
    printRow(rasp_cam);

    return 0;
}
*/