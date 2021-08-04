// #include "utils/sensors/DHT11.h"
// #include "utils/shared_memory.h"

// #include <Rcpp.h>

// #include <time.h> //for time
// #include <unistd.h>

// #include <R.h>
// #include <Rdefines.h>
// #include <R_ext/Utils.h>

// void assign_time(char* datetime) {
//     time_t rawtime;
//     struct tm * timeinfo;

//     time (&rawtime);
//     timeinfo = localtime(&rawtime);
//     strcpy(datetime,asctime(timeinfo));
// }

// void check_interrupt_fn(void *dummy) {
//     R_CheckUserInterrupt(); 
// }

// int pending_interrupt() {
//     return !(R_ToplevelExec(check_interrupt_fn, NULL));
// }


// void freeMemory() {
//     Rcpp::NumericVector x;
//     Rcpp::Rcout << "Freeing" << std::endl;
//     my_object* status = attach_memory_block(BLOCK_SIZE, NUM_BLOCKS+1);
//     for (int i = 0; i < NUM_BLOCKS; ++i) {
//         if (free_memory_block(i)) {    
//             Rcpp::Rcout <<"Free block: " << i << std::endl;
//         } else {
//             Rcpp::Rcout <<"Could not free block: " << i << std::endl;
//         }
//     }
    
//     if (free_memory_block(NUM_BLOCKS+1)) {    
//         Rcpp::Rcout <<"Free block: pointer"  << std::endl;
//     } else {
//         Rcpp::Rcout <<"Could not free block: pointer" << std::endl;
//     }
// }


// void writeMemory(Rcpp::LogicalVector nh = 0, Rcpp::NumericVector w = 0) {
//     DHT11_Operator dht;
//     int chk;
//     bool nohup = nh[0];// Not implemented yet
//     int wait = w[0];
//     Rcpp::Rcout << "Writer program is starting..." << std::endl;
//     my_object* status = attach_memory_block(BLOCK_SIZE, NUM_BLOCKS+1);
//     my_object* block;
//     int i = 0;
//     while (!pending_interrupt()) {
//         if (i == NUM_BLOCKS) {
//             i = 0;
//             status->complete = true;
//         }
//         sleep(wait);
//         chk = dht.readDHT11(DHT11_Pin);
//         if (chk == DHTLIB_OK) {
//             Rcpp::Rcout << "DHT11, OK!" << std::endl;
            
//             block = attach_memory_block(BLOCK_SIZE,i);
//             assign_time(block->datetime);
//             block->temp = dht.temperature;
//             block->hum = dht.humidity;
//             Rcpp::Rcout << "Humidity is " << dht.humidity << "%," << "\t Temperature is " << dht.temperature << "*C" << std::endl;

//             detach_memory_block(block);
//         } else if (chk == DHTLIB_ERROR_CHECKSUM){
//             Rcpp::Rcout << "DHT11,NOT OK! ERROR CHECKSUM" << std::endl;
//         } else if (chk == DHTLIB_ERROR_TIMEOUT) {
//             Rcpp::Rcout << "DHT11,NOT OK! ERROR TIMEOUT" << std::endl;
//         } else {
//             Rcpp::Rcout << "DHT11,NOT OK! ERROR UNKNOWN" << std::endl;
//         }
        
//         status->cur_id = i;
//         ++i;
//     }

//     detach_memory_block(status);
//     Rcpp::Rcout << "End Writing" << std::endl;
//     freeMemory();
// }


// Rcpp::List readMemory(Rcpp::IntegerVector read_block = 1) {
//     Rcpp::StringVector datetime;
//     Rcpp::NumericVector temp;
//     Rcpp::NumericVector hum;
//     int n = read_block[0];
//     if(n > NUM_BLOCKS) {
//         Rcpp::stop("size of reading blocks is greater than memory capacity.");
//     }
    
//     my_object* status = attach_memory_block(BLOCK_SIZE,NUM_BLOCKS+1);
//     int cur = status->cur_id;
//     my_object* block;
    
//     for (int i =0 ; i < n;++i) {
//         if (cur-i < 0) {
//             if (status->complete) {
//                 cur = NUM_BLOCKS+i-1;
//             } else {
//                 Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
//                 break;
//             }
            
//         }
//         block = attach_memory_block(BLOCK_SIZE,cur-i);
//         // Rcpp::Rcout << "Reading ";
//         // Rcpp::Rcout << cur -i << std::endl;
//         // Rcpp::Rcout << block->datetime << " ";
//         // Rcpp::Rcout << "Temp = " << block->temp << ", ";
//         // Rcpp::Rcout << "Hum = " << block->hum << std::endl;

//         datetime.push_back(block->datetime);
//         temp.push_back(block->temp);
//         hum.push_back(block->hum);
//     }
//     Rcpp::List L = Rcpp::List::create(
//                         Rcpp::Named("datetime") = datetime,
//                         Rcpp::Named("temperature") = temp,
//                         Rcpp::Named("humidity") = hum
//                     );
//     return L;
// }

