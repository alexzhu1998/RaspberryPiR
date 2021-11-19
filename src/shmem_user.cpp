#include "utils/DHT11.h"
#include "utils/RPiCam.h"
#include "utils/PhotoRes.h"
#include "utils/MQ2.h"
// #include "utils/shared_memory.h"


// Each Sensor is wrapped with an R function. There are writeMemory, readMemory, freeMemory and scanPointer. 
// SENSOR_writeMemory 
//      timeDelay - Amount of delay in ms for each interval
//      pin information - Various GPIO Pin Required for each circuit 
// SENSOR_readMemory 
//      n which is the number of block memory
// SENSOR_freeMemory 
// SENSOR_scanPointer

// [[Rcpp::export]]
void DHT11_writeMemory(Rcpp::NumericVector timeDelay = 1000,Rcpp::NumericVector pin = 0) {
    
    DHT11 sensor;
    sensor.timeBetweenAcquisition = timeDelay[0];
    sensor.writeMemory(pin[0]);
}

// [[Rcpp::export]]
Rcpp::List DHT11_readMemory(Rcpp::NumericVector n = 1) {
    DHT11 sensor;
    return sensor.readMemory(n[0]);
}

// [[Rcpp::export]]
void DHT11_freeMemory() {
    SharedMemory sharedmem(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH,SHM_FREE,DATA_FREE);
    sharedmem.freeMemory();
}

// [[Rcpp::export]]
Rcpp::NumericVector DHT11_scanPointer() {
    SharedMemory sharedmem(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH,SHM_SCAN,DATA_DHT11);
    Rcpp::NumericVector x;
    x = Rcpp::NumericVector::create(sharedmem.retrieve_DataPtrIndex());
    // Rcpp::Rcout << x << std::endl;
    return x;
}

// [[Rcpp::export]]
void PhotoRes_writeMemory(Rcpp::NumericVector timeDelay = 1000,Rcpp::NumericVector pin = 7) {
    // Is it possible to do this without having to pass the parameter every time?
    PhotoRes sensor;
    sensor.timeBetweenAcquisition = timeDelay[0];
    sensor.writeMemory(pin[0]);
}

// [[Rcpp::export]]
Rcpp::List PhotoRes_readMemory(Rcpp::NumericVector n = 5) {
    PhotoRes sensor;
    return sensor.readMemory(n[0]);
}

// [[Rcpp::export]]
void PhotoRes_freeMemory() {
    SharedMemory sharedmem(PHOTORES_SHM_PATH,PHOTORES_SHM_PTR_PATH,SHM_FREE,DATA_FREE);
    sharedmem.freeMemory();
}

// [[Rcpp::export]]
Rcpp::NumericVector PhotoRes_scanPointer() {
    SharedMemory sharedmem(PHOTORES_SHM_PATH,PHOTORES_SHM_PTR_PATH,SHM_SCAN,DATA_PHOTORES);
    Rcpp::NumericVector x;
    x = Rcpp::NumericVector::create(sharedmem.retrieve_DataPtrIndex());
    // Rcpp::Rcout << x << std::endl;
    return x;
}

// [[Rcpp::export]]
void MQ2_writeMemory(Rcpp::NumericVector timeDelay = 1000, Rcpp::NumericVector SPICLK = 14, Rcpp::NumericVector SPIMISO = 13, Rcpp::NumericVector SPIMOSI = 12, Rcpp::NumericVector SPICS = 10, Rcpp::NumericVector mq2_dpin= 25, Rcpp::NumericVector mq2_apin= 0) {
    // Is it possible to do this without having to pass the parameter every time?
    MQ2 sensor;
    sensor.timeBetweenAcquisition = timeDelay[0];
    sensor.writeMemory(SPICLK[0], SPIMISO[0], SPIMOSI[0], SPICS[0], mq2_dpin[0], mq2_apin[0]);
}

// [[Rcpp::export]]
Rcpp::List MQ2_readMemory(Rcpp::NumericVector n = 1) {
    MQ2 sensor;
    return sensor.readMemory(n[0]);
}

// [[Rcpp::export]]
void MQ2_freeMemory() {
    SharedMemory sharedmem(MQ2_SHM_PATH,MQ2_SHM_PTR_PATH,SHM_FREE,DATA_FREE);
    sharedmem.freeMemory();
}

// [[Rcpp::export]]
Rcpp::NumericVector MQ2_scanPointer() {
    SharedMemory sharedmem(MQ2_SHM_PATH,MQ2_SHM_PTR_PATH,SHM_SCAN,DATA_MQ2);
    Rcpp::NumericVector x;
    x = Rcpp::NumericVector::create(sharedmem.retrieve_DataPtrIndex());
    // Rcpp::Rcout << x << std::endl;
    return x;
}

// [[Rcpp::export]]
void RPiCam_writeMemory(Rcpp::NumericVector timeDelay = 1000) {
    RPiCam sensor(WIDTH,HEIGHT,CHANNELS);
    sensor.timeBetweenAcquisition = timeDelay[0];
    sensor.writeMemory();
}

// [[Rcpp::export]] 
Rcpp::List RPiCam_readMemory(Rcpp::NumericVector n = 5) {
    RPiCam sensor(WIDTH,HEIGHT,CHANNELS);
    return sensor.readMemory(n[0]);
}

// [[Rcpp::export]]
void RPiCam_freeMemory() {
    SharedMemory sharedmem(RASPICAM_SHM_PATH,RASPICAM_SHM_PTR_PATH,SHM_FREE,DATA_FREE);
    sharedmem.freeMemory();
}

// [[Rcpp::export]]
Rcpp::NumericVector RPiCam_scanPointer() {
    SharedMemory sharedmem(RASPICAM_SHM_PATH,RASPICAM_SHM_PTR_PATH,SHM_SCAN,DATA_RASPICAM);
    Rcpp::NumericVector x;
    x = Rcpp::NumericVector::create(sharedmem.retrieve_DataPtrIndex());
    // Rcpp::Rcout << x << std::endl;
    return x;
}

// [[Rcpp::export]]
void testing_writeMemory() {
    SharedMemory sharedmem("/samplepath","/sampleptrpath",SHM_WRITE, DATA_PHOTORES);
    
    
    // Rcpp::Rcout << sizeof(source_data_ptr) << std::endl;
    // Rcpp::Rcout << sizeof(DataPtr) << std::endl;
    // sharedmem.map_DataPtr();
    // sharedmem.map_DataBlock1();
    // sharedmem.map_data_obj(source_data_ptr.allocated_memory,&source_data_obj);
    
    int i = 0;
    for (;!pending_interrupt();i++) {
        if (i == BLOCK_LENGTH) {
            i = 0;
            sharedmem.dp->complete = true;
        }
        Rcpp::Rcout << "tick ";
        sharedmem.db1->data[i]= (double)i;
        Rcpp::Rcout << "tock ";
        sharedmem.dp->cur_index = i;
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
Rcpp::List testing_readMemory(Rcpp::NumericVector x = 5) {
    SharedMemory sharedmem("/samplepath","/sampleptrpath",SHM_READ,DATA_PHOTORES);
    // DataBlock source_data_obj(2,BLOCK_LENGTH,REGULAR_SENSOR_TYPE);
     
    // DataPtr dp1(BLOCK_LENGTH);
    int n = x[0];
    
    
    for (int i =0; i < std::min(n,sharedmem.dp->cur_index); i++) {
        Rcpp::Rcout << i << "Data points: " <<  sharedmem.db1->data[i] << std::endl;
    }
    
    
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
    SharedMemory sharedmem("/samplepath","/sampleptrpath",SHM_FREE,DATA_FREE);
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

