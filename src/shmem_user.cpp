#include "utils/DHT11.h"
#include "utils/RPiCam.h"
// #include "utils/shared_memory.h"




// // [[Rcpp::export]]
// void DHT11_writeMemory(Rcpp::NumericVector pin = 0) {
//     // Is it possible to do this without having to pass the parameter every time?
//     DHT11 sensor(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH);
//     sensor.writeMemory(pin[0]);
// }

// // [[Rcpp::export]]
// Rcpp::List DHT11_readMemory(Rcpp::NumericVector n = 1) {
//     DHT11 sensor(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH);
//     return sensor.readMemory(n[0]);
// }

// // [[Rcpp::export]]
// void DHT11_freeMemory() {
//     DHT11 sensor(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH);
//     sensor.freeMemory();
// }

// // [[Rcpp::export]]
// void RPiCam_writeMemory() {
//     RPiCam sensor(RPICAM_SHM_PATH,RPICAM_SHM_PTR_PATH);
//     sensor.writeMemory();
// }

// // [[Rcpp::export]]
// void RPiCam_freeMemory() {
//     RPiCam sensor(RPICAM_SHM_PATH,RPICAM_SHM_PTR_PATH);
//     sensor.freeMemory();
// }

// // [[Rcpp::export]] 
// Rcpp::List RPiCam_readMemory(Rcpp::NumericVector n = 1) {
//     RPiCam sensor(RPICAM_SHM_PATH,RPICAM_SHM_PTR_PATH);
//     return sensor.readMemory(n[0]);
// }


// [[Rcpp::export]]
void testing_writeMemory() {
    SharedMemory sharedmem("/samplepath","/sampleptrpath",SHM_WRITE);
    
    DataPtr source_data_ptr(2,BLOCK_LENGTH);
    DataBlock source_data_obj(2,BLOCK_LENGTH,REGULAR_SENSOR_TYPE);
    
    Rcpp::Rcout << sizeof(source_data_ptr) << std::endl;
    Rcpp::Rcout << sizeof(DataPtr) << std::endl;
    sharedmem.map_data_ptr(sizeof(DataPtr),&source_data_ptr);
    sharedmem.map_data_obj(source_data_ptr.allocated_memory,&source_data_obj);
    Rcpp::Rcout << sharedmem.data_ptr->allocated_memory << std::endl;

    // DataPtr dp1(BLOCK_LENGTH);
    // DataPtr* ptr_obj = &dp1;
    
    int i = 0;
    for (;!pending_interrupt();i++) {
        if (i == sharedmem.data_ptr->num_data_points * sharedmem.data_ptr->block_length) {
            i = 0;
            sharedmem.data_ptr->complete = true;
        }
        Rcpp::Rcout << "tick ";
        sharedmem.data_obj->sensor_data[i]= (double)i;
        Rcpp::Rcout << "tock ";
        sharedmem.data_ptr->cur_index = i;
        sleep(1);
        
    }
    // sharedmem.open_write();
    // sharedmem.map_write();
    // Rcpp::Rcout << sizeof(db1) << std::endl;
    // Rcpp::Rcout << sizeof(DataBlock*) << std::endl;
    // Rcpp::Rcout << sizeof(*data_obj) << std::endl;
    // data_obj = static_cast<DataBlock*>(mmap(NULL,sizeof(db1), PROT_READ | PROT_WRITE,MAP_SHARED,sharedmem.fd,0));
    // if (data_obj == MAP_FAILED)
    //     handle_error("mmap data_obj");
    // if (ftruncate(sharedmem.fd, sizeof(db1)) == -1)           /* To obtain file size */
    //     handle_error("open_write fd ftruncate");
    // ptr_obj = static_cast<DataPtr*>(mmap(NULL,sizeof(*ptr_obj), PROT_READ | PROT_WRITE,MAP_SHARED,sharedmem.fd_ptr,0));
    // if (ptr_obj == MAP_FAILED)
    //     handle_error("mmap");
    // if (ftruncate(sharedmem.fd_ptr, sizeof(*ptr_obj)) == -1)           /* To obtain file size */
    //     handle_error("open_write fd_ptr ftruncate");

    // data_obj->data1[0] = 123;
    // data_obj->data1[1] = 234;
    sharedmem.freeMemory();
}

// [[Rcpp::export]]
Rcpp::List testing_readMemory() {
    SharedMemory sharedmem("/samplepath","/sampleptrpath",SHM_READ);
    DataBlock source_data_obj(2,BLOCK_LENGTH,REGULAR_SENSOR_TYPE);
     
    // DataPtr dp1(BLOCK_LENGTH);
    
    sharedmem.retrieve_data_ptr(sizeof(DataPtr));
    Rcpp::Rcout << "Allocated Memory: "<< sharedmem.data_ptr->allocated_memory << std::endl;
    
    sharedmem.retrieve_data_obj(sharedmem.data_ptr->allocated_memory);
    memcpy(&source_data_obj,sharedmem.data_obj,sharedmem.data_ptr->allocated_memory);
    Rcpp::Rcout<< source_data_obj.success << std::endl;
    
    // for (int i =0; i < std::min(5,sharedmem.data_ptr->cur_index); i++) {
    //     Rcpp::Rcout << i << "Data points: " <<  sharedmem.data_obj->sensor_data[i] << std::endl;
    // }
    
    
    // DataBlock* data_obj = &db1;
    // DataPtr* ptr_obj = &dp1;

    // sharedmem.init_read();
    // Rcpp::Rcout << data_obj->data1[0] << std::endl;
    // Rcpp::Rcout << data_obj->data1[1] << std::endl;

    return Rcpp::List::create(
        Rcpp::Named("temperature") = Rcpp::NumericVector(1)
    );
}

// [[Rcpp::export]]
void testing_freeMemory() {
    SharedMemory sharedmem("/samplepath","/sampleptrpath",2);
    sharedmem.freeMemory();
}

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

