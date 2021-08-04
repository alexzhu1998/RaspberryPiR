#include "sensors_control.h"

time_t Sensor::get_raw_time() {
    time_t rawtime;
    time(&rawtime);
    return rawtime;
}

std::string Sensor::to_time_string(time_t rawtime) {
    char datetime[DATE_STRING_SIZE];
    
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strcpy(datetime,asctime(timeinfo));
    
    return std::string(datetime);
}

// void Sensor::check_interrupt_fn(void *dummy) {
//     R_CheckUserInterrupt(); 
// }

// int Sensor::pending_interrupt() {
//     return !(R_ToplevelExec(check_interrupt_fn, NULL));
// }