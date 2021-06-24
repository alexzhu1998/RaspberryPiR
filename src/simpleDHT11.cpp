#include <Rcpp.h>
#include <wiringPi.h>
#include <stdint.h>

#include "DHTLib/DHT11.h"




// [[Rcpp::export]]
Rcpp::NumericVector testingDHT () {
    DHT dht;
    int chk;
    Rcpp::Rcout << "Hello!, Program is starting..." << std::endl;
    Rcpp::NumericVector y = Rcpp::NumericVector::create(0,0);
    chk = dht.readDHT11(DHT11_Pin);
    if (chk == DHTLIB_OK) {
        Rcpp::Rcout << "DHT11, OK!" << std::endl;
        y = Rcpp::NumericVector::create(dht.humidity,dht.temperature);
        Rcpp::Rcout << "Humidity is " << dht.humidity << "%," << "\t Temperature is " << dht.temperature << "*C" << std::endl;
    } else if (chk == DHTLIB_ERROR_CHECKSUM){
        Rcpp::Rcout << "DHT11,NOT OK! ERROR CHECKSUM" << std::endl;
    } else if (chk == DHTLIB_ERROR_TIMEOUT) {
        Rcpp::Rcout << "DHT11,NOT OK! ERROR TIMEOUT" << std::endl;
    } else {
        Rcpp::Rcout << "DHT11,NOT OK! ERROR UNKNOWN" << std::endl;
    }

    return y;
}

