#include <Rcpp.h>
#include "DHTLib/DHT11.h"
#include <time.h> //for time
#include "shmem/shared_memory.h"
#include <R.h>
#include <Rdefines.h>
#include <R_ext/Utils.h>

void assign_time(char* datetime) {
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
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
Rcpp::NumericVector writeMemory() {
    DHT dht;
    int chk;
    Rcpp::NumericVector x = Rcpp::NumericVector::create(0,0);
    Rcpp::Rcout << "Writer program is starting..." << std::endl;
    my_object* status = attach_memory_block(BLOCK_SIZE, NUM_BLOCKS+1);
    my_object* block;
    int i = 0;
    while (!pending_interrupt()) {
        if (i == NUM_BLOCKS) {
            i = 0;
            status->complete = true;
        }

        chk = dht.readDHT11(DHT11_Pin);
        if (chk == DHTLIB_OK) {
            Rcpp::Rcout << "DHT11, OK!" << std::endl;
            x = Rcpp::NumericVector::create(dht.humidity,dht.temperature);
            
            block = attach_memory_block(BLOCK_SIZE,i);
            assign_time(block->datetime);
            block->temp = dht.temperature;
            block->hum = dht.humidity;
            Rcpp::Rcout << "Humidity is " << dht.humidity << "%," << "\t Temperature is " << dht.temperature << "*C" << std::endl;

            detach_memory_block(block);
        } else if (chk == DHTLIB_ERROR_CHECKSUM){
            Rcpp::Rcout << "DHT11,NOT OK! ERROR CHECKSUM" << std::endl;
        } else if (chk == DHTLIB_ERROR_TIMEOUT) {
            Rcpp::Rcout << "DHT11,NOT OK! ERROR TIMEOUT" << std::endl;
        } else {
            Rcpp::Rcout << "DHT11,NOT OK! ERROR UNKNOWN" << std::endl;
        }
        
        status->cur_id = i;
        ++i;
    }

    detach_memory_block(status);
    Rcpp::Rcout << "End Writing" << std::endl;

    return x;
}

// [[Rcpp::export]]
Rcpp::NumericVector readMemory() {
    Rcpp::NumericVector x;
    my_object* status = attach_memory_block(BLOCK_SIZE,NUM_BLOCKS+1);
    my_object* block;
    block = attach_memory_block(BLOCK_SIZE,status->cur_id);
    Rcpp::Rcout << "Reading ";
    Rcpp::Rcout << status->cur_id << ": ";
    Rcpp::Rcout << block->datetime << " ";
    Rcpp::Rcout << "Temp = " << block->temp << ", ";
    Rcpp::Rcout << "Hum = " << block->hum << std::endl;
    x = Rcpp::NumericVector::create(block->hum,block->temp);
    return x;
}

// [[Rcpp::export]]
void freeMemory() {
    Rcpp::NumericVector x;
    Rcpp::Rcout << "Freeing" << std::endl;
    my_object* status = attach_memory_block(BLOCK_SIZE, NUM_BLOCKS+1);
    for (int i = 0; i <= NUM_BLOCKS; ++i) {
        if (free_memory_block(i)) {    
            Rcpp::Rcout <<"Free block: " << i << std::endl;
        } else {
            Rcpp::Rcout <<"Could not free block: " << i << std::endl;
        }
    }
    
    if (free_memory_block(NUM_BLOCKS+1)) {    
        Rcpp::Rcout <<"Free block: pointer"  << std::endl;
    } else {
        Rcpp::Rcout <<"Could not free block: pointer" << std::endl;
    }
}