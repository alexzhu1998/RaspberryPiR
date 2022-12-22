#include "PhotoRes.h"



PhotoRes_Operator::PhotoRes_Operator(int _pin) : pin(_pin) {
    wiringPiSetup();
}

int PhotoRes_Operator::readPhotoRes() {
    pinMode(pin,OUTPUT);
    digitalWrite(pin,LOW);
    delay(100);

    pinMode(pin,INPUT);
    clock_t t= clock(), diff=0;
    
    while (digitalRead(pin) == LOW) {
        diff = clock() - t;
    }
    PhoR_time_to_charge = (double) diff/ CLOCKS_PER_SEC;
    return PHOR_SENSOR_OK;
}

void PhotoRes::writeMemory(int pin, int len) {
    SharedMemory sharedmem(PHOTORES_SHM_PATH,PHOTORES_SHM_PTR_PATH,SHM_WRITE);
    int n_datapts = 1;
    DataPtr source_dp(n_datapts,len);
    DataBlock source_db(n_datapts,len,REGULAR_SENSOR_TYPE);
    
    sharedmem.map_data_ptr(&source_dp);
    sharedmem.map_data_obj(source_dp.allocated_memory,&source_db);

    int i = 0, chk;
    TimeVar start;
    PhotoRes_Operator phoR(pin);
    for (;!pending_interrupt();i++) {
        if (i == sharedmem.dp->block_length) {
            i = 0;
            sharedmem.dp->complete = 1;
        }
        Rcpp::Rcout <<  "Block No: " << i << std::endl;
        start = timeNow();
        // assigning values
        chk = phoR.readPhotoRes();
        Rcpp::Rcout << "Time to charge is " << phoR.PhoR_time_to_charge << " ms"<< std::endl; 
        sharedmem.db->raw_time[i] = get_raw_time();
        Rcpp::Rcout << "Time: " << to_time_string(sharedmem.db->raw_time[i]) << std::endl;
        sharedmem.db->sensor_data[n_datapts*i] = static_cast<double>(phoR.PhoR_time_to_charge);
        double elapsed = intervalDuration(timeNow()-start);
        Rcpp::Rcout << "Time to charge is " << phoR.PhoR_time_to_charge << " ms"<< std::endl; 
        millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        sharedmem.dp->cur_index = i;
        
    }
    Rcpp::Rcout << "End Writing" << std::endl;
    sharedmem.freeMemory();
}

Rcpp::List PhotoRes::readMemory(int n) {
    SharedMemory sharedmem(PHOTORES_SHM_PATH,PHOTORES_SHM_PTR_PATH,SHM_READ);
    DataBlock* dbRead = new DataBlock(sharedmem.dp->num_data_points,sharedmem.dp->block_length,REGULAR_SENSOR_TYPE); // no need to delete because this gets overwritten by mmap
    sharedmem.retrieve_data_ptr(sizeof(DataPtr)); //retrieve memory from data ptr (which is statically assigned) 
    Rcpp::Rcout << "Block Length: " << sharedmem.dp->block_length << " Number of Data Pts: " << sharedmem.dp->num_data_points << std::endl;
    Rcpp::Rcout << "Allocated Memory: "<< sharedmem.dp->allocated_memory << std::endl;
    sharedmem.retrieve_data_obj(sharedmem.dp->allocated_memory, dbRead); // retrieve memory from data block (which is dynamically assigned)
    Rcpp::Rcout << "Sensor data offset is " << sharedmem.db->sensor_data_offset << std::endl;

    // Checking error
    if (n > sharedmem.dp->block_length) {
        Rcpp::Rcout << "Error: Read Memory Block Length exceeded Write Memory Block Length" << std::endl; 
        return Rcpp::List::create(
            Rcpp::Named("Error") = Rcpp::NumericVector(1)
        );
    } else if (n <= 0) {
        Rcpp::Rcout << "Error: Read Memory Block Length negative" << std::endl; 
        return Rcpp::List::create(
            Rcpp::Named("Error") = Rcpp::NumericVector(1)
        );
    }

    int cur = sharedmem.dp->cur_index;
    
    if (cur - n < 0 && sharedmem.dp->complete == 0) {
        Rcpp::Rcout << "Error: Length of Read Memory Block exceeded current position" << std::endl;
        return Rcpp::List::create(
            Rcpp::Named("Error") = Rcpp::NumericVector(1)
        );
    }
    
    Rcpp::CharacterVector datetime;
    Rcpp::NumericVector time_to_charge;

    int i = cur - n;
    i += cur - n < 0 ? sharedmem.dp->block_length : 0;
    for (; i != cur ;++i) {
        if (i == sharedmem.dp->block_length) {
            if (sharedmem.dp->complete) {
                i = 0;
            } else {
                Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                break;
            }
        }
        std::string t = to_time_string(dbRead->raw_time[i]);
        Rcpp::Rcout << t << " " << dbRead->sensor_data[i] << std::endl;
        datetime.push_back(t);
        time_to_charge.push_back(static_cast<double>(dbRead->sensor_data[i]));
    }
    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("time_to_charge") = time_to_charge
    ); 
}
