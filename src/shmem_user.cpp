#include <Rcpp.h>
#include "DHTLib/DHT11.h"
#include <time.h> //for time
#include <unistd.h>
#include "shared_memory.h"
#include <R.h>
#include <Rdefines.h>
#include <R_ext/Utils.h>
#include <vector>
#include <chrono> // for measuring real execution time
#include <thread> // for sleep


time_t get_raw_time() {
    time_t rawtime;
    
    time(&rawtime);
    return rawtime;
}

void convert_to_string(char*datetime, time_t rawtime) {
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    strcpy(datetime,asctime(timeinfo));
}

void check_interrupt_fn(void *dummy) {
    R_CheckUserInterrupt(); 
}

int pending_interrupt() {
    return !(R_ToplevelExec(check_interrupt_fn, NULL));
}

void assign_DHT_block(int pin, int sensor ,DHT& dht, data_obj<double,BLOCK_LENGTH>* block,int &index) {
    int chk;
    if (sensor == 0) {
        chk = dht.readDHT11(pin);
    } else if (sensor == 1) {
        chk = dht.readPhoRSensor(pin);
    }
    
    if (chk == SENSOR_OK) {
        Rcpp::Rcout << "DHT11, OK!" << std::endl;
        
        block->raw_time[index] = get_raw_time();
        if (sensor == 0) {
            block->data1[index] = dht.temperature;
            block->data2[index] = dht.humidity;
            Rcpp::Rcout << "Humidity is " << block->data2[index]  << "%," << "\t Temperature is " << block->data1[index]  << "*C" << std::endl;
        } else if (sensor == 1) {
            block->data1[index] = dht.PhoR_time_to_charge;
            Rcpp::Rcout << "Time to charge is " << block->data1[index] << " s"<< std::endl;
        }

        
    } else if (chk == DHTLIB_ERROR_CHECKSUM){
        Rcpp::Rcout << "DHT11,NOT OK! ERROR CHECKSUM" << std::endl;
    } else if (chk == SENSOR_ERROR_TIMEOUT) {
        Rcpp::Rcout << "SENSOR ,NOT OK! ERROR TIMEOUT" << std::endl;
    } else {
        Rcpp::Rcout << "SENSOR ,NOT OK! ERROR UNKNOWN" << std::endl;
    }
}

int WarningHelperIdentifySensor(Rcpp::StringVector sensor) {
    
    std::vector<std::string> sensorMapping = {"DHT11", "PhoR"};
    for (int s = 0; s < sensorMapping.size();s++) {
        if (sensor[0] == sensorMapping[s]) {
            return s;
        }
    }
    
    Rcpp::stop("Sensor Not Found, Terminating...");
    return -1;
}
// [[Rcpp::export]]
void freeMemory(Rcpp::StringVector sensor = "DHT11") {
    int sens = WarningHelperIdentifySensor(sensor);

    const char* shmpath;
    const char* shmpath_pointer;
    switch(sens) {
        case 0:
            shmpath = "/DHT11";
            shmpath_pointer = "/DHT11_ptr";
            break;
        case 1:
            shmpath = "/PhoR";
            shmpath_pointer = "/PhoR_ptr";
            break;
        default:
            shmpath = NULL;
            shmpath_pointer = NULL;
    }
    Rcpp::Rcout <<"Free block" << std::endl;
    shm_unlink(shmpath);
    Rcpp::Rcout <<"Free block: pointer"  << std::endl;
    shm_unlink(shmpath_pointer);
}

// [[Rcpp::export]]
void writeMemory(Rcpp::StringVector sensor = "DHT11",Rcpp::NumericVector pin = 0, Rcpp::NumericVector w = 1000) {
    
    double interval = w[0];

    int sens = WarningHelperIdentifySensor(sensor);
    
    Rcpp::Rcout << "Writer program is starting..." << std::endl;
    const char* shmpath;
    const char* shmpath_pointer;
    switch(sens) {
        case 0:
            shmpath = "/DHT11";
            shmpath_pointer = "/DHT11_ptr";
            break;
        case 1:
            shmpath = "/PhoR";
            shmpath_pointer = "/PhoR_ptr";
            break;
        default:
            shmpath = NULL;
            shmpath_pointer = NULL;
    }

    int fd = shm_open(shmpath,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    int fd_ptr = shm_open(shmpath_pointer,O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1 || fd_ptr == -1){
        shm_unlink(shmpath);
        shm_unlink(shmpath_pointer);
        handle_error("open");
    }

    //// truncate file to precisely length bytes
    if (ftruncate(fd, sizeof(data_obj<double,BLOCK_LENGTH>)) == -1)           /* To obtain file size */
        handle_error("fstat");
    if (ftruncate(fd_ptr, sizeof(data_ptr)) == -1)           /* To obtain file size */
        handle_error("fstat");

    data_obj<double,BLOCK_LENGTH>* data_block = static_cast<data_obj<double,BLOCK_LENGTH>*>(mmap(NULL,sizeof(*data_block), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    if (data_block == MAP_FAILED) 
        handle_error("mmap");
    
    TimeVar start;
    DHT dht;
    
    data_ptr* status = static_cast<data_ptr*>(mmap(NULL,sizeof(*status), PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    
    int i = 0;
    for (;!pending_interrupt();++i) {
        if (i == BLOCK_LENGTH) {
            i = 0;
            status->complete = true;
        }
        
        start = timeNow();
        // to fix
        assign_DHT_block(pin[0], sens, dht,data_block,i);
        double elapsed = intervalDuration(timeNow()-start);
        
        millisleep((unsigned int)std::max((interval-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        status->index = i;
    }
    

    Rcpp::Rcout << "End Writing" << std::endl;
    freeMemory(sensor);
}

Rcpp::List retrieve_DHT_block(int sensor, int n) {
    const char* shmpath;
    const char* shmpath_pointer;
    switch(sensor) {
        case 0:
            shmpath = "/DHT11";
            shmpath_pointer = "/DHT11_ptr";
            break;
        case 1:
            shmpath = "/PhoR";
            shmpath_pointer = "/PhoR_ptr";
            break;
        default:
            shmpath = NULL;
            shmpath_pointer = NULL;
    }
    int fd = shm_open(shmpath, O_RDWR, S_IRUSR|S_IWUSR);
    int fd_ptr = shm_open(shmpath_pointer, O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == -1||fd_ptr == -1){
        handle_error("fd fd_ptr");
    }

    data_ptr* status = static_cast<data_ptr*>(mmap(NULL,sizeof(*status), PROT_READ | PROT_WRITE,MAP_SHARED,fd_ptr,0));
    if (status == MAP_FAILED) 
        handle_error("mmap");

    int cur = status->index;
    
    struct data_obj<double,BLOCK_LENGTH>* data_block = static_cast<data_obj<double,BLOCK_LENGTH>*>(mmap(NULL,sizeof(*data_block), PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));
    
    char time_string[DATE_STRING_SIZE];
    Rcpp::StringVector datetime;
    if (sensor == 0) {
        Rcpp::NumericVector temp;
        Rcpp::NumericVector hum;
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
            temp.push_back(data_block->data1[cur-i]);
            hum.push_back(data_block->data2[cur-i]);
        }
        return Rcpp::List::create(
                            Rcpp::Named("datetime") = datetime,
                            Rcpp::Named("temperature") = temp,
                            Rcpp::Named("humidity") = hum
                        );
    } else if (sensor == 1) {
        Rcpp::NumericVector time_to_c;
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
            time_to_c.push_back(data_block->data1[cur-i]);
        }
        return Rcpp::List::create(
                            Rcpp::Named("datetime") = datetime,
                            Rcpp::Named("charge_time") = time_to_c
                        );
    }
    
    return Rcpp::List::create(Rcpp::Named("empty") = 0);
    
}

// [[Rcpp::export]]
Rcpp::List readMemory(Rcpp::IntegerVector read_block = 1, Rcpp::StringVector sensor = "DHT11") {
    int sens = WarningHelperIdentifySensor(sensor);
    int n = read_block[0];
    if(n > BLOCK_LENGTH) {
        Rcpp::stop("size of reading blocks is greater than memory capacity.");
    }
    
    return retrieve_DHT_block(sens,n);

}

