#include <Rcpp.h>
#include "DHT11.h"
#include <time.h> //for time
#include <unistd.h>
#include "shared_memory.h"
#include <R.h>
#include <Rdefines.h>
#include <R_ext/Utils.h>
#include <vector>
#include <chrono> // for measuring real execution time
#include <thread> // for sleep


typedef std::chrono::high_resolution_clock::time_point TimeVar;

#define intervalDuration(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
#define millisleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

#define KEY_1 9876
#define DATE_STRING_SIZE 30

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

// [[Rcpp::export]]
void freeMemory() {
    Rcpp::NumericVector x;
    Rcpp::Rcout << "Freeing" << std::endl;
    my_object<double,double>*  status;
    attach_memory_block(status, BLOCK_SIZE, NUM_BLOCKS+1,KEY_1);
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (free_memory_block(i,KEY_1)) {    
            Rcpp::Rcout <<"Free block: " << i << std::endl;
        } else {
            Rcpp::Rcout <<"Could not free block: " << i << std::endl;
        }
    }
    
    if (free_memory_block(NUM_BLOCKS+1,KEY_1)) {    
        Rcpp::Rcout <<"Free block: pointer"  << std::endl;
    } else {
        Rcpp::Rcout <<"Could not free block: pointer" << std::endl;
    }
    if (!detach_memory_block(status)) Rcpp::stop("Memory can't be detached");
}

template <typename T, typename U>
void assign_DHT_block(int pin, int sensor ,DHT& dht, my_object<T,U>* block,int &index, int key) {
    int chk;
    if (sensor == 0) {
        chk = dht.readDHT11(pin);
    } else if (sensor == 1) {
        chk = dht.readPhoRSensor(pin);
    }
    
    if (chk == SENSOR_OK) {
        Rcpp::Rcout << "DHT11, OK!" << std::endl;
        
        attach_memory_block(block,BLOCK_SIZE,index, key);
        // assign_time(block->datetime);
        
        block->raw_time = get_raw_time();
        if (sensor == 0) {
            block->data1 = dht.temperature;
            block->data2 = dht.humidity;
            Rcpp::Rcout << "Humidity is " << block->data2  << "%," << "\t Temperature is " << block->data1  << "*C" << std::endl;
        } else if (sensor == 1) {
            block->data1 = dht.PhoR_time_to_charge;
            Rcpp::Rcout << "Time to charge is " << block->data1 << " s"<< std::endl;
        }

        if (!detach_memory_block(block)) Rcpp::stop("Memory can't be detached");
    } else if (chk == DHTLIB_ERROR_CHECKSUM){
        Rcpp::Rcout << "DHT11,NOT OK! ERROR CHECKSUM" << std::endl;
    } else if (chk == SENSOR_ERROR_TIMEOUT) {
        Rcpp::Rcout << "SENSOR ,NOT OK! ERROR TIMEOUT" << std::endl;
    } else {
        Rcpp::Rcout << "SENSOR ,NOT OK! ERROR UNKNOWN" << std::endl;
    }
}

int WarningHelperIdentifySensor(Rcpp::StringVector sensor) {
    int s = 0;
    std::vector<std::string> sensorMapping = {"DHT11", "PhoR"};
    for (; s < sensorMapping.size();s++) {
        if (sensor[0] == sensorMapping[s]) {
            return s;
        }
    }
    Rcpp::stop("Sensor Not Found, Terminating...");
    return -1;
}

// [[Rcpp::export]]
void writeMemory(Rcpp::StringVector sensor = "DHT11",Rcpp::NumericVector pin = 0, Rcpp::LogicalVector nh = 0, Rcpp::NumericVector w = 1000) {
    
    bool nohup = nh[0];// Not implemented yet
    double interval = w[0];

    int sens = WarningHelperIdentifySensor(sensor);
    
    Rcpp::Rcout << "Writer program is starting..." << std::endl;
    my_object<double,double>* status;
    attach_memory_block(status, BLOCK_SIZE, NUM_BLOCKS+1,KEY_1);
    
    TimeVar start;
    DHT dht;
    my_object<double,double>* data_block;
    int i = 0;
    for (;!pending_interrupt();++i) {
        if (i == NUM_BLOCKS) {
            i = 0;
            status->complete = true;
        }
        
        start = timeNow();
        assign_DHT_block(pin[0], sens, dht,data_block,i,KEY_1);
        double elapsed = intervalDuration(timeNow()-start);
        
        millisleep((unsigned int)std::max((interval-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        status->cur_id = i;
    }

    
    
    if (!detach_memory_block(status)) Rcpp::stop("Memory can't be detached");
    Rcpp::Rcout << "End Writing" << std::endl;
    freeMemory();
}

Rcpp::List retrieve_DHT_block(int sensor, int n, int key) {
    my_object<double,double>* status;
    attach_memory_block(status,BLOCK_SIZE,NUM_BLOCKS+1,key);
    int cur = status->cur_id;
    my_object<double,double>* data_block;
    char time_string[DATE_STRING_SIZE];
    Rcpp::StringVector datetime;
    if (sensor == 0) {
        Rcpp::NumericVector temp;
        Rcpp::NumericVector hum;
        for (int i =0 ; i < n;++i) {
            if (cur-i < 0) {
                if (status->complete) {
                    cur = NUM_BLOCKS+i-1;
                } else {
                    Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                    break;
                }
            }
            attach_memory_block(data_block,BLOCK_SIZE,cur-i,key);
            convert_to_string(time_string,data_block->raw_time);
            datetime.push_back(time_string);
            temp.push_back(data_block->data1);
            hum.push_back(data_block->data2);
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
                    cur = NUM_BLOCKS+i-1;
                } else {
                    Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                    break;
                }
            }
            attach_memory_block(data_block,BLOCK_SIZE,cur-i,key);
            convert_to_string(time_string,data_block->raw_time);
            datetime.push_back(time_string);
            time_to_c.push_back(data_block->data1);
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
    if(n > NUM_BLOCKS) {
        Rcpp::stop("size of reading blocks is greater than memory capacity.");
    }
    
    return retrieve_DHT_block(sens,n,KEY_1);

}

