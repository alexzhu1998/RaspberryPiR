#include <Rcpp.h>
#include <wiringPi.h>
#include <stdint.h>

#include "DHTLib/DHT11.h"




// [[Rcpp::export]]
void testingDHT (Rcpp::NumericVector pin = 0) {
    DHT dht;
    int chk;
    Rcpp::Rcout << "Hello!, Program is starting..." << std::endl;
    // default pin is GPIO 17 Pin
    chk = dht.readDHT11(pin[0]);
    if (chk == SENSOR_OK) {
        Rcpp::Rcout << "DHT11, OK!" << std::endl;
        Rcpp::Rcout << "Humidity is " << dht.humidity << "%," << "\t Temperature is " << dht.temperature << "*C" << std::endl;
    } else if (chk == DHTLIB_ERROR_CHECKSUM){
        Rcpp::Rcout << "DHT11,NOT OK! ERROR CHECKSUM" << std::endl;
    } else if (chk == SENSOR_ERROR_TIMEOUT) {
        Rcpp::Rcout << "DHT11,NOT OK! ERROR TIMEOUT" << std::endl;
    } else {
        Rcpp::Rcout << "DHT11,NOT OK! ERROR UNKNOWN" << std::endl;
    }

}

