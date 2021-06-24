#include <stdio.h>
#include "simpleDHT11.hpp"
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
IntegerVector testDHT11 (IntegerVector x = 0) {
    DHT dht;
    int chk;
    printf("Program is starting ... \n");

    chk = dht.readDHT11Once(DHT11_Pin);
    if (chk == DHTLIB_OK) {
        printf("DHT11, OK! \n");
    } else {
        printf("DHT11, Not OK! chk: %d\n", chk);
    }
    printf("Humidity is %.2f %%, \t Temperature is %.2f *C\n\n",dht.humidity, dht.temperature);
    return x;
}