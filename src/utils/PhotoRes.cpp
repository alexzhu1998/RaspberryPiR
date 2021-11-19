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

void PhotoRes::writeMemory(int pin) {
    SharedMemory sharedmem(PHOTORES_SHM_PATH,PHOTORES_SHM_PTR_PATH,SHM_WRITE,DATA_PHOTORES);
    DataPtr* data_ptr = sharedmem.dp;
    DataBlock1* data_obj = sharedmem.db1;

    int i = 0;
    int chk;
    TimeVar start;
    PhotoRes_Operator phoR(pin);
    for (;!pending_interrupt();++i) {
        if (i == data_ptr->block_length) {
            i = 0;
            data_ptr->complete = true;
        }
        Rcpp::Rcout <<  "Block No: " << data_ptr->cur_index << std::endl;
        start = timeNow();
        // assigning values
        chk = phoR.readPhotoRes();
        data_obj->raw_time[i] = get_raw_time();
        data_obj->data[i] = static_cast<double>(phoR.PhoR_time_to_charge);
        
        double elapsed = intervalDuration(timeNow()-start);
        Rcpp::Rcout << "Time to charge is " << phoR.PhoR_time_to_charge << " ms"<< std::endl; 
        millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        data_ptr->cur_index = i;
        
    }
    Rcpp::Rcout << "End Writing" << std::endl;

    Rcpp::Rcout << "Freeing Memory" << std::endl;
    sharedmem.freeMemory();
}

Rcpp::List PhotoRes::readMemory(int n) {
    SharedMemory sharedmem(PHOTORES_SHM_PATH,PHOTORES_SHM_PTR_PATH,SHM_READ,DATA_PHOTORES);
    DataPtr* data_ptr = sharedmem.dp;
    DataBlock1* data_obj = sharedmem.db1;
    Rcpp::CharacterVector datetime;
    Rcpp::NumericVector time_to_charge;
    

    int cur = data_ptr->cur_index;
    int block_length = data_ptr->block_length;
    Rcpp::Rcout << block_length << std::endl;
    
    for (int i =0 ; i < n;++i) {
        if (cur-i < 0) {
            if (data_ptr->complete) {
                cur = block_length+i-1;
            } else {
                Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                break;
            }
        }
        
        std::string t = to_time_string(data_obj->raw_time[cur-i]);
        
        datetime.push_back(t);
        time_to_charge.push_back(static_cast<double>(data_obj->data[cur-i]));
        
    }

    // sharedmem.unmap_DataPtr(data_ptr);
    // sharedmem.unmap_DataBlock1(data_obj);
    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("time_to_charge") = time_to_charge
    ); 
}