#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <Rcpp.h>
#include <wiringPi.h>
#include <time.h> //for time
#include <chrono> // for measuring real execution time
#include <thread> // for sleep
#include <R_ext/Utils.h> // R_CheckUserInterrupt()
#include <Rdefines.h> //R_ToplevelExec()
#include <R.h>



typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define intervalDuration(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
#define millisleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define DATE_STRING_SIZE 30

class Sensor {
public:
    
    time_t rawtime;
    time_t get_raw_time();
    std::string to_time_string(time_t rawtime);
    // void check_interrupt_fn(void *dummy);
    // int pending_interrupt();

    int timeBetweenAcquisition;

    // virtual void info() = 0;
    // virtual void writeMemory(int pin) = 0;
    // virtual Rcpp::List readMemory(int n) = 0;
    // void freeMemory();
    // virtual void killProcess() = 0;
};

#endif // SENSOR_CONTROL_H