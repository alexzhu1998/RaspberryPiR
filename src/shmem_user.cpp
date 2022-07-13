#include "utils/DHT11.h"
#include "utils/PhotoRes.h"
#include "utils/RPiCam.h"
#include "utils/MQ2.h"
// #include "utils/shared_memory.h"




// [[Rcpp::export]]
void DHT11_writeMemory(Rcpp::NumericVector pin = 0, Rcpp::IntegerVector length = 100, Rcpp::IntegerVector timeDelay = 1000) {
    // Is it possible to do this without having to pass the parameter every time?
    DHT11 sensor;
    sensor.timeBetweenAcquisition = timeDelay[0];
    sensor.writeMemory(pin[0], length[0]);
}

// [[Rcpp::export]]
Rcpp::List DHT11_readMemory(Rcpp::NumericVector n = 1) {
    DHT11 sensor;
    return sensor.readMemory(n[0]);
}

// [[Rcpp::export]]
void DHT11_freeMemory() {
    SharedMemory sharedmem(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH,SHM_FREE);
    sharedmem.freeMemory();
}

// [[Rcpp::export]]
void PhotoRes_writeMemory(Rcpp::NumericVector pin = 7, Rcpp::IntegerVector length = 100, Rcpp::IntegerVector timeDelay = 1000) {
    // Is it possible to do this without having to pass the parameter every time?
    PhotoRes sensor;
    sensor.timeBetweenAcquisition = timeDelay[0];
    sensor.writeMemory(pin[0], length[0]);
}

// [[Rcpp::export]]
Rcpp::List PhotoRes_readMemory(Rcpp::NumericVector n = 1) {
    PhotoRes sensor;
    return sensor.readMemory(n[0]);
}

// [[Rcpp::export]]
void PhotoRes_freeMemory() {
    SharedMemory sharedmem(PHOTORES_SHM_PATH,PHOTORES_SHM_PTR_PATH,SHM_FREE);
    sharedmem.freeMemory();
}


// // [[Rcpp::export]]
// void RPiCam_writeMemory(Rcpp::IntegerVector length = 100, Rcpp::IntegerVector timeDelay = 1000) {
//     RPiCam sensor;
//     sensor.writeMemory(length[0]);
// }

// // [[Rcpp::export]]
// void RPiCam_freeMemory() {
//     RPiCam sensor;
//     sensor.freeMemory();
// }

// // [[Rcpp::export]] 
// Rcpp::List RPiCam_readMemory(Rcpp::NumericVector n = 1) {
//     RPiCam sensor;
//     return sensor.readMemory(n[0]);
// }


/*
NOTES:
Isolated the problem to mmap, currently running tests to check if variables can be allocated dy
Testing if mmap works with the current dynamic memory allocation. The data stored is artificially generated.
IMPORTANT: Data is not being written into the shared memory space


TODO: 
Fix MMAP Dynamic Memory
Fix Action Upon Interrupt
Fix OOP
Fix Share with User Information  
*/

// [[Rcpp::export]]
void testing_writeMemory() {
    
    SharedMemory sharedmem("/samplepath","/sampleptrpath",SHM_WRITE);
    
    DataPtr source_dp(2,BLOCK_LENGTH);
    DataBlock source_db(2,BLOCK_LENGTH,REGULAR_SENSOR_TYPE);
    Rcpp::Rcout << sizeof(source_db) << " " << sizeof(DataBlock) << std::endl;
    Rcpp::Rcout << sizeof(source_dp) << " " << sizeof(DataPtr) << std::endl;
    
    sharedmem.map_data_ptr(&source_dp);
    sharedmem.map_data_obj(source_dp.allocated_memory,&source_db);
    Rcpp::Rcout << "Allocated Write Memory: " << sharedmem.dp->allocated_memory << std::endl;

    // DataPtr dp1(BLOCK_LENGTH);
    // DataPtr* ptr_obj = &dp1;
    
    int i = 0;
    for (;!pending_interrupt();i++) {
        if (i == sharedmem.dp->num_data_points * sharedmem.dp->block_length) {
            i = 0;
            sharedmem.dp->complete = true;
        }
        
        sharedmem.db->sensor_data[i]= (double)(i*i);
        
        sharedmem.dp->cur_index = i;
        Rcpp::Rcout << sharedmem.dp->cur_index << " " << sharedmem.db->sensor_data[i] <<" " << sharedmem.db->sensor_data << std::endl;
        Rcpp::Rcout << (double)(*(sharedmem.db->sensor_data + 5)) << std::endl;
        sleep(1);
        
    }
    // sharedmem.open_write();
    // sharedmem.map_write();
    // Rcpp::Rcout << sizeof(db1) << std::endl;
    // Rcpp::Rcout << sizeof(DataBlock*) << std::endl;
    // Rcpp::Rcout << sizeof(*db) << std::endl;
    // db = static_cast<DataBlock*>(mmap(NULL,sizeof(db1), PROT_READ | PROT_WRITE,MAP_SHARED,sharedmem.fd,0));
    // if (db == MAP_FAILED)
    //     handle_error("mmap db");
    // if (ftruncate(sharedmem.fd, sizeof(db1)) == -1)           /* To obtain file size */
    //     handle_error("open_write fd ftruncate");
    // ptr_obj = static_cast<DataPtr*>(mmap(NULL,sizeof(*ptr_obj), PROT_READ | PROT_WRITE,MAP_SHARED,sharedmem.fd_ptr,0));
    // if (ptr_obj == MAP_FAILED)
    //     handle_error("mmap");
    // if (ftruncate(sharedmem.fd_ptr, sizeof(*ptr_obj)) == -1)           /* To obtain file size */
    //     handle_error("open_write fd_ptr ftruncate");

    // db->data1[0] = 123;
    // db->data1[1] = 234;
    // sharedmem.freeMemory();
}

/*
Problem comes from here when I try to call from the shared memory block.
Questions for Pavlo: 
1. Why does the DataBlock need a new for readMemory?
2. 
*/

// [[Rcpp::export]]
Rcpp::List testing_readMemory() {
    SharedMemory sharedmem("/samplepath","/sampleptrpath",SHM_READ);
    DataBlock* dbRead = new DataBlock(2,BLOCK_LENGTH,REGULAR_SENSOR_TYPE); // no need to delete because this gets overwritten by mmap
    // DataBlock source_db(2,BLOCK_LENGTH,REGULAR_SENSOR_TYPE);
    
    sharedmem.retrieve_data_ptr(sizeof(DataPtr)); //retrieve memory from data ptr (which is statically assigned) 
    Rcpp::Rcout << "Block Length: " << sharedmem.dp->block_length << "Number of Data Pts: " << sharedmem.dp->num_data_points << std::endl;
    Rcpp::Rcout << "Allocated Memory: "<< sharedmem.dp->allocated_memory << std::endl;
    
    sharedmem.retrieve_data_obj(sharedmem.dp->allocated_memory, dbRead); // retrieve memory from data block (which is dynamically assigned)
    Rcpp::Rcout << "Sensor data offset is " << sharedmem.db->sensor_data_offset << std::endl;


    Rcpp::Rcout << sharedmem.dp->cur_index << std::endl;
    Rcpp::Rcout << sharedmem.db->success << std::endl;
    
    // Rcpp::Rcout << sharedmem.db->sensor_data[sharedmem.dp->cur_index] << std::endl;
    
    // memcpy(&source_data_obj,sharedmem.db,sharedmem.dp->allocated_memory);
    // Rcpp::Rcout<< source_data_obj.success << std::endl;
    
    for (int i =0; i < 50; i++) {
        
        Rcpp::Rcout << i << "Data points: " <<  dbRead->sensor_data[i]<< " " << dbRead->sensor_data << std::endl;
        Rcpp::Rcout << (double)(*(dbRead->sensor_data + 5)) << std::endl;
    }
    
    
    // DataBlock* db = &db1;
    // DataPtr* ptr_obj = &dp1;

    // sharedmem.init_read();
    // Rcpp::Rcout << db->data1[0] << std::endl;
    // Rcpp::Rcout << db->data1[1] << std::endl;

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

